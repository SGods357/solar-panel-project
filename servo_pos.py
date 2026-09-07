from main import azimuth, elevation

AZ_ZERO_OFFSET = 0  # Adjust this value based on your servo's zero position
EL_ZERO_OFFSET = 0  # Adjust this value based on your servo's zero position

AZ_GEAR_RATIO = 2  # Adjust this value based on your servo's gear ratio
EL_GEAR_RATIO = 0.5  # Adjust this value based on your servo's gear ratio

servo_angle_pan = ((azimuth - AZ_ZERO_OFFSET) % 360) / AZ_GEAR_RATIO
servo_angle_tilt = (elevation - EL_ZERO_OFFSET) / EL_GEAR_RATIO

print(f"Servo Angle Tilt: {servo_angle_tilt:.2f} degrees")
print(f"Servo Angle Pan: {servo_angle_pan:.2f} degrees")