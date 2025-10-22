import HumanData from "../model/HumanData.js";


let io;
// Function to inject io instance
const setSocketIO = (ioInstance) => {
  io = ioInstance;
};

// POST /api/human-data → store sensor data
const saveHumanData = async (req, res) => {
  try {
    const data = req.body;
    // console.log(" Received Human Data:", data);

    const newRecord = new HumanData({
      humanPresence: data.humanPresence,
      motion: data.motion,
      moveRange: data.moveRange,
      avgMoveRange: data.avgMoveRange,
      alertActive: data.alertActive
    });

    await newRecord.save();

    // ✅ Emit real-time update to all connected clients
    if (io) {
      io.emit("humanData", newRecord);
    //   console.log(" real-time update via socket.io");
    }

    res.status(200).json({ message: "Data saved successfully", data: newRecord });
  } catch (error) {
    console.error(" Error saving data:", error);
    res.status(500).json({ message: "Error saving data", error });
  }
};


// GET /api/human-data → get recent records
const getAllHumanData = async (req, res) => {
  try {
    const records = await HumanData.find().sort({ timestamp: -1 });
    res.status(200).json(records);
  } catch (error) {
    console.error("❌ Error fetching data:", error);
    res.status(500).json({ message: "Error fetching data", error });
  }
};


export { setSocketIO, saveHumanData, getAllHumanData };