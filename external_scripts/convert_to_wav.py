import pandas as pd
import numpy as np
import soundfile as sf

sample_rate = 8000

# Step 1: Load the data with timestamps
df = pd.read_csv("output.txt", names=["timestamp", "value"])
df["timestamp"] = pd.to_datetime(df["timestamp"])
df["value"] = pd.to_numeric(df["value"], errors="coerce")
df = df.dropna()

# Step 2: Convert to numpy array and normalize
samples = df["value"].to_numpy(dtype=np.float32)
samples -= samples.mean()  # Remove DC offset
samples /= np.max(np.abs(samples))  # Normalize to -1.0 to 1.0 range

print(f"Sample rate: {sample_rate} Hz")

# Step 4: Write to WAV file
sf.write("output.wav", samples, sample_rate)
print("Saved as output.wav!")
