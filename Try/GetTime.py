from datetime import datetime       # You can easily get the current date and time

now = datetime.now()

print(now.strftime('%Y%m%d%H%M%S')) # strftime() -> change to String  of the desired
