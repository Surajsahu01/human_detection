import React, { useState } from "react";
import { Toaster, toast } from "react-hot-toast";

export default function Settings() {
  const [installHeight, setInstallHeight] = useState(270);
  const [noHumanTime, setNoHumanTime] = useState(60);

  const handleSave = (e) => {
    e.preventDefault();
    // For now, just show a toast
    toast.success("Settings saved successfully!");
    // Later: POST these values to your server API
  };

  return (
    <div className="p-4 sm:p-6 space-y-6 min-h-screen bg-gray-50">
      <Toaster />

      {/* Header */}
      <h1 className="text-2xl sm:text-3xl font-semibold text-gray-800">
        Sensor Settings
      </h1>

      {/* Form */}
      <form
        onSubmit={handleSave}
        className="bg-white shadow-md rounded-lg p-6 space-y-4 max-w-md"
      >
        {/* Installation Height */}
        <div>
          <label className="block text-gray-700 font-medium mb-1">
            Sensor Installation Height (cm)
          </label>
          <input
            type="number"
            value={installHeight}
            onChange={(e) => setInstallHeight(e.target.value)}
            className="w-full border rounded-lg px-3 py-2 focus:outline-none focus:ring-2 focus:ring-blue-500"
            min={50}
            max={500}
          />
        </div>

        {/* No Human Timeout */}
        <div>
          <label className="block text-gray-700 font-medium mb-1">
            No Human Timeout (seconds)
          </label>
          <input
            type="number"
            value={noHumanTime}
            onChange={(e) => setNoHumanTime(e.target.value)}
            className="w-full border rounded-lg px-3 py-2 focus:outline-none focus:ring-2 focus:ring-blue-500"
            min={10}
            max={3600}
          />
        </div>

        {/* Save Button */}
        <button
          type="submit"
          className="w-full bg-green-600 text-white px-4 py-2 rounded-lg hover:bg-green-700 transition"
        >
          Save Settings
        </button>
      </form>
    </div>
  );
}
