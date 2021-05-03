package main

import (
	"github.com/ericsperano/meteo/bitmap2a2lr/cmd"
	log "github.com/sirupsen/logrus"
)

func main() {
	if err := cmd.Execute(); err != nil {
		log.Fatal(err)
	}
}
