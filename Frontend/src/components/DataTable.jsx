import React from "react";

function DataTable({ data }) {
  return (
    <div className="bg-white shadow rounded-xl overflow-hidden">
      {/* 📱 Mobile View: Card Layout */}
      <div className="block md:hidden space-y-4 p-4">
        {data.length > 0 ? (
          data.map((item, i) => (
            <div
              key={i}
              className="border rounded-lg p-3 bg-gray-50 hover:bg-gray-100 transition"
            >
              <div className="flex justify-between text-sm text-gray-600">
                {/* <span>{new Date(item.timestamp).toLocaleString("en-US")}</span> */}
                <span>
                  {new Date(item.timestamp).toLocaleString("en-GB",{
                    hour12: true
                  })}
                </span>
                <span
                  className={`px-2 py-1 rounded-full text-xs ${
                    item.alertActive
                      ? "bg-red-100 text-red-700"
                      : "bg-green-100 text-green-700"
                  }`}
                >
                  {item.alertActive ? "Active" : "Normal"}
                </span>
              </div>

              <div className="mt-2 text-sm space-y-1">
                <p>
                  <span className="font-medium text-gray-700">Presence: </span>
                  {item.humanPresence === "Detected" ? "Yes" : "No"}
                </p>
                <p>
                  <span className="font-medium text-gray-700">Motion: </span>
                  {item.motion || "Unknown"}
                </p>
                <p>
                  <span className="font-medium text-gray-700">Move Range: </span>
                  {item.moveRange}
                </p>
                <p>
                  <span className="font-medium text-gray-700">
                    Avg Move Range:{" "}
                  </span>
                  {item.avgMoveRange}
                </p>
              </div>
            </div>
          ))
        ) : (
          <p className="text-center text-gray-500 py-4">No data available</p>
        )}
      </div>

      {/* 💻 Desktop View: Table Layout with scrollable tbody */}
      <div className="hidden md:block max-h-[400px] overflow-y-auto">
        <table className="min-w-full text-sm text-left border-collapse">
          <thead className="bg-gray-100 border-b sticky top-0 z-10 text-gray-700">
            <tr>
              <th className="py-3 px-4">Time</th>
              <th className="py-3 px-4">Presence</th>
              <th className="py-3 px-4">Motion</th>
              <th className="py-3 px-4">Move Range</th>
              <th className="py-3 px-4">Avg Move Range</th>
              <th className="py-3 px-4">Alert</th>
            </tr>
          </thead>
          <tbody>
            {data.length > 0 ? (
              data.map((item, i) => (
                <tr key={i} className="border-b hover:bg-gray-50">
                  {/* <td className="py-2 px-4">
                    {new Date(item.timestamp).toLocaleString("en-US")}
                  </td> */}
                  <td className="py-2 px-4">
                    {new Date(item.timestamp).toLocaleString("en-GB", {
                      hour12: true
                    })}
                  </td>

                  <td className="py-2 px-4">
                    {item.humanPresence === "Detected" ? "Yes" : "No"}
                  </td>
                  <td className="py-2 px-4">{item.motion || "Unknown"}</td>
                  <td className="py-2 px-4">{item.moveRange}</td>
                  <td className="py-2 px-4">{item.avgMoveRange}</td>
                  <td className="py-2 px-4">
                    <span
                      className={`px-3 py-1 rounded-full text-xs ${
                        item.alertActive
                          ? "bg-red-100 text-red-700"
                          : "bg-green-100 text-green-700"
                      }`}
                    >
                      {item.alertActive ? "Active" : "Normal"}
                    </span>
                  </td>
                </tr>
              ))
            ) : (
              <tr>
                <td colSpan="6" className="text-center py-4 text-gray-500">
                  No data available
                </td>
              </tr>
            )}
          </tbody>
        </table>
      </div>
    </div>
    
  );
}

export default DataTable;
