import mongoose from "mongoose";

const HumanDataSchema = new mongoose.Schema({
  humanPresence: 
  { 
    type: String, 
    required: true 
},
  motion: 
  { 
    type: String, 
    required: true 
},
  moveRange: 
  { 
    type: Number, 
    required: true
 },
  avgMoveRange: 
  { 
    type: Number, 
    required: true 
},
  alertActive: 
  { 
    type: Boolean,
     required: true 
    },
  timestamp: 
  { 
    type: Date, 
    default: Date.now,
    index: { expires: 3600 } 
 }
});

const HumanData = mongoose.model("HumanData", HumanDataSchema);

export default HumanData;
