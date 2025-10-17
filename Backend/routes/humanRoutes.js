import express from "express";
import { getAllHumanData, saveHumanData } from "../controllers/humanController.js";

const router = express.Router();

router.post("/human-data", saveHumanData );
router.get("/human-data", getAllHumanData);

export default router;