import { useState, useEffect, useRef } from "react";
import StatusCard from "../components/StatusCard";
import DataTable from "../components/DataTable";
import { axiosClient } from "../api/axiosClient";
import { io } from "socket.io-client";

const API_URL = import.meta.env.VITE_API_URL;
function Dashboard() {
  const [data, setData] = useState([]);
  const [latest, setLatest] = useState(null);
  const [loading, setLoading] = useState(false);

  const socketRef = useRef(null);

  // Fetch data from server
  const fetchData = async () => {
    try {
      setLoading(true);
      const res = await axiosClient.get("/api/human-data");

      // Sort descending by timestamp
      const sortedData = res.data.sort(
        (a, b) => new Date(b.timestamp) - new Date(a.timestamp)
      );

      setData(sortedData);
      setLatest(sortedData[0]);
    } catch (error) {
      console.error("Error fetching data:", error);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchData();

    // Initialize socket
    socketRef.current = io(API_URL, {
      transports: ["websocket", "polling"],
    });

    socketRef.current.on("connect", () => {
      // console.log("Socket connected:", socketRef.current.id);
    });

    // Handle new human data in real-time
    socketRef.current.on("humanData", (newData) => {
      setData((prev) => [newData, ...prev]);
      setLatest(newData);

    });

    socketRef.current.on("disconnect", () => {
      console.log("Socket disconnected");
    });

    return () => {
      if (socketRef.current) socketRef.current.disconnect();
    };
  }, []);

  return (
     <div className="p-2 sm:p-2 min-h-screen bg-gray-50 flex justify-center">
      <div className="w-full max-w-7xl border-2 border-gray-300 rounded-2xl shadow-lg p-6 space-y-6 bg-white">
      

      {/* Header */}
      <div className="flex flex-col sm:flex-row justify-between items-start sm:items-center gap-3">
        <h1 className="text-2xl sm:text-3xl font-semibold text-gray-800">
          Dashboard
        </h1>

        {/* Refresh Button */}
        <button
          onClick={fetchData}
          disabled={loading}
          className={`w-full sm:w-auto flex items-center justify-center gap-2 bg-blue-600 text-white px-4 py-2 rounded-lg hover:bg-blue-700 transition ${
            loading ? "opacity-70 cursor-not-allowed" : ""
          }`}
        >
          {loading ? (
            <>
              <svg
                className="animate-spin h-5 w-5 text-white"
                xmlns="http://www.w3.org/2000/svg"
                fill="none"
                viewBox="0 0 24 24"
              >
                <circle
                  className="opacity-25"
                  cx="12"
                  cy="12"
                  r="10"
                  stroke="currentColor"
                  strokeWidth="4"
                ></circle>
                <path
                  className="opacity-75"
                  fill="currentColor"
                  d="M4 12a8 8 0 018-8v4a4 4 0 00-4 4H4z"
                ></path>
              </svg>
              <span>Refreshing...</span>
            </>
          ) : (
            "Refresh Data"
          )}
        </button>
      </div>

      {/* Status Cards */}
      {latest ? (
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4">
          <StatusCard
            title="Human Presence"
            value={latest.humanPresence === "Detected" ? "Detected" : "Not Detected"}
            color={latest.humanPresence === "Detected" ? "border-green-500 text-green-600" : "border-red-500 text-red-600"}
          />
          <StatusCard
            title="Motion"
            value={latest.motion || "Unknown"}
            color="border-indigo-500"
          />
          <StatusCard
            title="Alert"
            value={latest.alertActive ? "Active" : "Normal"}
            color={latest.alertActive ? "border-red-500 text-red-600" : "border-green-500 text-green-600"}
          />
        </div>
      ) : (
        <p className="text-gray-500 text-center py-4">
          Loading latest data...
        </p>
      )}

      {/* Data Table */}
      <div className="bg-white shadow-md rounded-lg overflow-x-auto p-4">
        <DataTable data={data} />
      </div>
      </div>
    </div>
  );
}

export default Dashboard;



// import { useState, useEffect, useRef } from "react";
// import StatusCard from "../components/StatusCard";
// import DataTable from "../components/DataTable";
// import { axiosClient } from "../api/axiosClient";
// import { io } from "socket.io-client";

// const API_URL = import.meta.env.API_URL;

// function Dashboard() {
//   const [data, setData] = useState([]);
//   const [latest, setLatest] = useState(null);
//   const [loading, setLoading] = useState(false);

//   const socketRef = useRef(null);

//   // Fetch data from server
//   const fetchData = async () => {
//     try {
//       setLoading(true);
//       const res = await axiosClient.get("/api/human-data");

//       // Sort descending by timestamp
//       const sortedData = res.data.sort(
//         (a, b) => new Date(b.timestamp) - new Date(a.timestamp)
//       );

//       setData(sortedData);
//       setLatest(sortedData[0] || null);
//     } catch (error) {
//       console.error("Error fetching data:", error);
//     } finally {
//       setLoading(false);
//     }
//   };

//   useEffect(() => {
//     fetchData();

//     // Initialize socket
//     socketRef.current = io(API_URL, {
//       transports: ["websocket", "polling"],
//     });

//     socketRef.current.on("connect", () => {
//       // console.log("Socket connected:", socketRef.current.id);
//     });

//     // Handle new human data in real-time
//     socketRef.current.on("humanData", (newData) => {
//       setData((prev) => [newData, ...prev]);
//       setLatest(newData);
//     });

//     socketRef.current.on("disconnect", () => {
//       console.log("Socket disconnected");
//     });

//     return () => {
//       if (socketRef.current) socketRef.current.disconnect();
//     };
//   }, []);

//   return (
//     <div className="p-2 sm:p-2 min-h-screen bg-gray-50 flex justify-center">
//       <div className="w-full max-w-7xl border-2 border-gray-300 rounded-2xl shadow-lg p-6 space-y-6 bg-white">
//         {/* Header */}
//         <div className="flex flex-col sm:flex-row justify-between items-start sm:items-center gap-3">
//           <h1 className="text-2xl sm:text-3xl font-semibold text-gray-800">
//             Dashboard
//           </h1>

//           {/* Refresh Button */}
//           <button
//             onClick={fetchData}
//             disabled={loading}
//             className={`w-full sm:w-auto flex items-center justify-center gap-2 bg-blue-600 text-white px-4 py-2 rounded-lg hover:bg-blue-700 transition ${
//               loading ? "opacity-70 cursor-not-allowed" : ""
//             }`}
//           >
//             {loading ? (
//               <>
//                 <svg
//                   className="animate-spin h-5 w-5 text-white"
//                   xmlns="http://www.w3.org/2000/svg"
//                   fill="none"
//                   viewBox="0 0 24 24"
//                 >
//                   <circle
//                     className="opacity-25"
//                     cx="12"
//                     cy="12"
//                     r="10"
//                     stroke="currentColor"
//                     strokeWidth="4"
//                   ></circle>
//                   <path
//                     className="opacity-75"
//                     fill="currentColor"
//                     d="M4 12a8 8 0 018-8v4a4 4 0 00-4 4H4z"
//                   ></path>
//                 </svg>
//                 <span>Refreshing...</span>
//               </>
//             ) : (
//               "Refresh Data"
//             )}
//           </button>
//         </div>

//         {/* Status Cards */}
//         {latest ? (
//           <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4">
//             <StatusCard
//               title="Human Presence"
//               value={
//                 latest.humanPresence === "Detected" ? "Detected" : "Not Detected"
//               }
//               color={
//                 latest.humanPresence === "Detected"
//                   ? "border-green-500 text-green-600"
//                   : "border-red-500 text-red-600"
//               }
//             />
//             <StatusCard
//               title="Motion"
//               value={latest.motion || "Unknown"}
//               color="border-indigo-500"
//             />
//             <StatusCard
//               title="Alert"
//               value={latest.alertActive ? "Active" : "Normal"}
//               color={
//                 latest.alertActive
//                   ? "border-red-500 text-red-600"
//                   : "border-green-500 text-green-600"
//               }
//             />
//           </div>
//         ) : loading ? (
//           <p className="text-gray-500 text-center py-4">
//             Loading latest data...
//           </p>
//         ) : (
//           <p className="text-gray-500 text-center py-4">No data available</p>
//         )}

//         {/* Data Table */}
//         <div className="bg-white shadow-md rounded-lg overflow-x-auto p-4">
//           {loading ? (
//             <p className="text-center text-gray-500 py-4">Loading data...</p>
//           ) : data.length === 0 ? (
//             <p className="text-center text-gray-500 py-4">No data available</p>
//           ) : (
//             <DataTable data={data} />
//           )}
//         </div>
//       </div>
//     </div>
//   );
// }

// export default Dashboard;
