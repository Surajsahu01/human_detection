import React from "react";
import { AlertTriangle, CheckCircle, Activity } from "lucide-react";

function StatusCard({ title, value, color }) {
  // Add simple icons based on title (optional but improves UX)
  const getIcon = () => {
    if (title.toLowerCase().includes("alert"))
      return <AlertTriangle className="text-red-500 w-6 h-6" />;
    if (title.toLowerCase().includes("motion"))
      return <Activity className="text-indigo-500 w-6 h-6" />;
    if (title.toLowerCase().includes("presence"))
      return <CheckCircle className="text-green-500 w-6 h-6" />;
    return null;
  };

  return (
    <div
      className={`flex flex-col sm:flex-row sm:items-center justify-between border-2 ${color} rounded-2xl p-4 sm:p-5 shadow-sm bg-white transition transform hover:scale-[1.02] hover:shadow-md`}
    >
      {/* Left: Title & Icon */}
      <div className="flex items-center gap-3">
        <div className="hidden sm:block">{getIcon()}</div>
        <h3 className="text-gray-600 text-base sm:text-md font-medium text-center sm:text-left">
          {title}
        </h3>
      </div>

      {/* Right: Value */}
      <p className={`text-lg sm:text-xl font-bold text-gray-800 mt-2 sm:mt-0 text-center sm:text-right break-words ${color}`}>
        {value}
      </p>
    </div>
  );
}

export default StatusCard;
