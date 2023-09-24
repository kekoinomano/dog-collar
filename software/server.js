const fs = require("fs");
const https = require("https");
const path = require("path");
const express = require("express");
const WebSocket = require("ws");

const WS_PORT = 8888;

// Tus rutas de certificados SSL
const privateKey = fs.readFileSync('YOUR-PATH/privkey.pem', 'utf8');
const certificate = fs.readFileSync('YOUR-PATH/cert.pem', 'utf8');
const ca = fs.readFileSync('YOUR-PATH/chain.pem', 'utf8');

const credentials = {
  key: privateKey,
  cert: certificate,
  ca: ca
};



const httpsServer = https.createServer(credentials);

const wsServer = new WebSocket.Server({ server: httpsServer });

// array of connected websocket clients
let connectedClients = [];
let isListening = false;


wsServer.on("connection", (ws, req) => {
  console.log("Cliente conectado");
  connectedClients.push(ws);

  ws.on("message", (data) => {


    // Determinamos si el mensaje es un blob o un string
    if (typeof data === "string") {
      if (data === "START_LISTENING") {
          isListening = true;
          console.log("WebSocket is now listening.");
      } 
      // Si recibimos un mensaje que dice "STOP_LISTENING", cambiamos el estado a false.
      else if (data === "STOP_LISTENING") {
          isListening = false;
          console.log("WebSocket stopped listening.");
      }
      console.log("Tipo de dato recibido: String");
      console.log("Valor: ", data);
    } else if (data instanceof Buffer) {
      console.log("Tipo de dato recibido: Buffer");
      console.log("Longitud del Buffer:", data.length);

    }
    connectedClients.forEach((ws, i) => {
      if (ws.readyState === ws.OPEN) {
        ws.send(data);
      } else {
        connectedClients.splice(i, 1);
      }
    });
  });


  ws.on("close", () => {
    console.log("Cliente desconectado");
  });
});


httpsServer.listen(WS_PORT, () => {
  console.log(`HTTPS and WebSocket seguro escuchando en wss://examiun.com:${WS_PORT}`);
});
