package common

import (
	"fmt"
	"time"
)

const (
	SeverityError   = "ERROR"
	SeverityWarning = "WARNING"
	SeverityInfo    = "INFO"
	SeverityDebug   = "DEBUG"
)

func LogF(severity, format string, args ...interface{}) {
	// for now, just logging to stdout
	fmt.Printf("%s [%s] %s\n", time.Now().Format(time.RFC3339), severity, fmt.Sprintf(format, args...))
}
