package api

import (
	"fmt"
	"io"
	"math/rand"
	"net/http"
	"strings"
	"sync"

	"github.com/danielcbailey/HydroponicsProject/server/common"
)

var authMutex sync.Mutex
var tokens map[string]bool

func initAuth() {
	tokens = make(map[string]bool)
}

func checkAuth(w http.ResponseWriter, r *http.Request) bool {
	token := r.Header.Get("Authorization")
	if token == "" {
		w.WriteHeader(http.StatusUnauthorized)
		return false
	}

	authMutex.Lock()
	defer authMutex.Unlock()

	if !tokens[token] {
		w.WriteHeader(http.StatusUnauthorized)
		return false
	}

	return true
}

func handleAuthenticate(w http.ResponseWriter, r *http.Request) {
	// body will be plaintext password
	// if password matches, generate token and return it
	// otherwise, return 401

	body, err := io.ReadAll(r.Body)
	if err != nil {
		common.LogF(common.SeverityError, "Error reading request body for authentication: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	if string(body) == common.GetConfig().SystemPassword {
		authMutex.Lock()
		defer authMutex.Unlock()

		token := generateToken()
		tokens[token] = true
		w.Write([]byte(token))
	} else {
		w.WriteHeader(http.StatusUnauthorized)
	}
}

func generateToken() string {
	// 40 characters of random hex
	builder := strings.Builder{}
	for i := 0; i < 40; i++ {
		builder.WriteString(fmt.Sprintf("%x", rand.Intn(16)))
	}

	return builder.String()
}
