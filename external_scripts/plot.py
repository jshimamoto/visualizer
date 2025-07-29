import pandas as pd
import matplotlib.pyplot as plt

# Load the data (adjust the path and separator if needed)
df = pd.read_csv("output.txt", names=["timestamp", "value"])

# Convert the timestamp to datetime (optional, but helpful for time plots)
df["timestamp"] = pd.to_datetime(df["timestamp"])

# Plot
plt.figure(figsize=(12, 6))
plt.plot(df["timestamp"], df["value"], label="Amplitude")
plt.xlabel("Time")
plt.ylabel("Amplitude")
plt.title("Audio Amplitude from Pico")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
