import React, { useEffect, useState } from "react";
import { axiosClient } from "../api/axiosClient";
import DataTable from "../components/DataTable";
import { Toaster, toast } from "react-hot-toast";

function Logs() {
  const [data, setData] = useState([]);
  const [loading, setLoading] = useState(false);

  const fetchLogs = async () => {
    try {
      setLoading(true);
      const res = await axiosClient.get("/human-data");
      setData(res.data.reverse());
    } catch (error) {
      toast.error("Failed to fetch logs.");
      console.error(error);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchLogs();
  }, []);

  return (
    <div className="p-4 sm:p-6 space-y-6 min-h-screen bg-gray-50">
      <Toaster />
      {/* Header */}
      <div className="flex flex-col sm:flex-row justify-between items-start sm:items-center gap-3">
        <h1 className="text-2xl sm:text-3xl font-semibold text-gray-800">
          Human Detection Logs
        </h1>
        <button
          onClick={fetchLogs}
          disabled={loading}
          className={`w-full sm:w-auto flex items-center justify-center gap-2 bg-blue-600 text-white px-4 py-2 rounded-lg hover:bg-blue-700 transition ${
            loading ? "opacity-70 cursor-not-allowed" : ""
          }`}
        >
          {loading ? (
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
          ) : (
            "Refresh Logs"
          )}
        </button>
      </div>

      {/* Logs Table / Card */}
      <div className="bg-white shadow-md rounded-lg overflow-x-auto p-4">
        <DataTable data={data} />
      </div>
    </div>
  );
}

export default Logs;