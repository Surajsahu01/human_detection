import express from "express";
import bodyParser from "body-parser";
import cors from "cors";
import http from "http";
import { Server } from 'socket.io';
import { setSocketIO } from "./controllers/humanController.js";
import humanRoutes from "./routes/humanRoutes.js";
import { connectDB } from "./db/db.js";
import dotenv from "dotenv";

dotenv.config();

const app = express();

const server = http.createServer(app);

// const io = new Server(server, {
//     cors: {
//         origin: "*",
//         methods: ["GET", "POST"],
//     },
// });

const io = new Server(server, {
  cors: {
    origin: process.env.FRONTEND, // ✅ no trailing slash
    methods: ["GET", "POST"],
    credentials: true // ✅ make sure this is inside cors
  }
});

app.use(cors({ origin: process.env.FRONTEND, credentials: true }));
app.use(bodyParser.json());

connectDB();

setSocketIO(io);

app.use("/api", humanRoutes);

io.on("connection", (socket) => {
    // console.log("Client connected:", socket.id);

    socket.on("disconnect", () => {
        // console.log("Client disconnected:", socket.id);
    });
});

const PORT = 5000 
server.listen(process.env.PORT || PORT, () => {
    console.log(`Server started on port ${PORT}`);
});