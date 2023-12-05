package hardwarecomms

import (
	"encoding/json"
	"errors"
	"sync"
	"sync/atomic"
	"time"

	"github.com/danielcbailey/HydroponicsProject/server/common"
	"go.bug.st/serial"
)

// Communication primitives
type command struct {
	JsonRPC string      `json:"jsonrpc"`
	Method  string      `json:"method"`
	Params  interface{} `json:"params"`
	ID      int64       `json:"id"`
}

type commandError struct {
	Code    int         `json:"code"`
	Message string      `json:"message"`
	Data    interface{} `json:"data"`
}

type commandResponse struct {
	JsonRPC string          `json:"jsonrpc"`
	Result  json.RawMessage `json:"result"`
	Error   commandError    `json:"error"`
	ID      int64           `json:"id"`
}

type notification struct {
	JsonRPC string      `json:"jsonrpc"`
	Method  string      `json:"method"`
	Params  interface{} `json:"params"`
}

// command structs
type commandSetBooleanState struct {
	State bool `json:"state"`
}

type commandSetTime struct {
	UnixTime int64 `json:"unixtime"`
}

type commandSetSchedule common.Schedule

type commandBlank struct {
	// no fields
}

// response structs
type acknoledgement struct {
	// no fields
}

// notification structs
type LogNotification struct {
	Severity    string `json:"severity"`
	Message     string `json:"message"`
	MessageType string `json:"message_type"`
}

// HardwareComms is the struct that handles the communication with the hardware
type HardwareComms struct {
	portMutex           sync.Mutex
	serialPort          serial.Port
	idCounter           int64
	idChanMap           map[int64]chan commandResponse
	NotificationHandler func(string, interface{})
	ReconnectHandler    func()
	commName            string
}

/**
 * Opens the serial port and returns a HardwareComms struct
 */
func OpenComms(commName string) (*HardwareComms, error) {
	port, err := serial.Open(commName, &serial.Mode{BaudRate: 115200})
	if err != nil {
		return nil, err
	}

	port.SetReadTimeout(1 * time.Second)

	comms := &HardwareComms{
		serialPort: port,
		idCounter:  0,
		commName:   commName,
		idChanMap:  make(map[int64]chan commandResponse),
	}

	go comms.readConn()
	return comms, nil
}

/**
 * Closes the serial port and releases the resources
 */
func (hc *HardwareComms) CloseComms() error {
	e := hc.serialPort.Close()
	hc.serialPort = nil
	return e
}

func (hc *HardwareComms) testAndTryToReconnect() bool {
	// should test if the connection is still active and if not, try to reconnect
	_, e := hc.serialPort.GetModemStatusBits()
	if e != nil {
		// try to reconnect
		hc.serialPort.Close()

		port, e := serial.Open(hc.commName, &serial.Mode{BaudRate: 115200})
		if e != nil {
			hc.serialPort = nil
			return false
		}
		hc.serialPort = port
		if hc.ReconnectHandler != nil {
			go hc.ReconnectHandler() // must be in a goroutine to avoid deadlock
		}
	}

	return true
}

func (hc *HardwareComms) readConn() {

	buildingBuf := make([]byte, 1024)
	trueLength := 0
	for hc.serialPort != nil {
		// read the response
		n, e := hc.serialPort.Read(buildingBuf[len(buildingBuf)-1024:])
		if e != nil {
			// try to reconnect
			if !hc.testAndTryToReconnect() {
				// if we can't reconnect, send a notification
				logEntry := LogNotification{
					Severity:    "ERROR",
					Message:     "Error reading from hardware: " + e.Error(),
					MessageType: "COMMUNICATION_FAILURE",
				}

				if hc.NotificationHandler != nil {
					hc.NotificationHandler("event/log", logEntry)
				}
				time.Sleep(1 * time.Minute)
			}

			time.Sleep(1 * time.Second)
			continue
		}

		trueLength += n
		buildingBuf = buildingBuf[:trueLength]

		n = 0
		for i, v := range buildingBuf {
			if v == '\n' {
				n = i
				break
			}
		}

		if n == 0 {
			// no newline found, continue reading
			buildingBuf = append(buildingBuf, make([]byte, 1024)...)
			continue
		}

		// parse the response
		var r commandResponse
		e = json.Unmarshal(buildingBuf[:n], &r)
		buildingBuf = make([]byte, 1024)
		trueLength = 0

		if e == nil && r.ID != 0 {
			// send the response to the waiting goroutine
			hc.portMutex.Lock()
			hc.idChanMap[r.ID] <- r
			delete(hc.idChanMap, r.ID)
			hc.portMutex.Unlock()

			continue
		}

		// trying to see if it is a notification
		var event notification
		e = json.Unmarshal(buildingBuf[:n], &event)
		if e == nil && event.Method != "" {
			// send the notification to the handler
			if hc.NotificationHandler != nil {
				hc.NotificationHandler(event.Method, event.Params)
			}
			continue
		}

		// if we got here, we have an error
		logEntry := LogNotification{
			Severity:    "ERROR",
			Message:     "Error parsing response from hardware: " + string(buildingBuf[:n]),
			MessageType: "COMMUNICATION_FAILURE",
		}

		if hc.NotificationHandler != nil {
			hc.NotificationHandler("event/log", logEntry)
		}
	}
}

/**
 * Sends a command to the hardware and returns the response
 */
func (hc *HardwareComms) sendCommand(cmd string, payload interface{}) (json.RawMessage, error) {
	// json serializing the payload
	c := command{
		JsonRPC: "2.0",
		Method:  cmd,
		Params:  payload,
		ID:      atomic.AddInt64(&hc.idCounter, 1),
	}

	b, e := json.Marshal(c)
	if e != nil {
		return nil, e
	}
	b = append(b, '\n')

	// sending the command
	hc.portMutex.Lock()
	if !hc.testAndTryToReconnect() {
		hc.portMutex.Unlock()
		return nil, errors.New("COMMUNICATION_FAILURE: unable to reconnect to hardware.")
	}

	_, e = hc.serialPort.Write(b)
	if e != nil {
		hc.portMutex.Unlock()
		return nil, e
	}
	hc.idChanMap[c.ID] = make(chan commandResponse)
	hc.portMutex.Unlock()

	// waiting for the response or timeout of 5 seconds
	select {
	case r := <-hc.idChanMap[c.ID]:
		if r.Error.Code != 0 {
			return nil, hc.decodeError(r)
		}

		return r.Result, nil
	case <-time.After(30 * time.Second):
		return nil, errors.New("TIMEOUT: timeout sending command " + cmd + " to hardware.")
	}
}

func (hc *HardwareComms) decodeError(r commandResponse) error {
	eType := "UNKNOWN_ERROR"
	switch r.Error.Code {
	case 100:
		eType = "INVALID_METHOD"
		break
	case 101:
		eType = "GENERIC_ERROR"
		break
	}

	return errors.New(eType + ": " + r.Error.Message)
}
