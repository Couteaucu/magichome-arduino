from magichome import MagicHomeApi

roomled = MagicHomeApi('192.168.8.11', 0)
backlight = MagicHomeApi('192.168.8.10', 0)
mainlamp = MagicHomeApi('192.168.8.12', 3)
#secondlamp = MagicHomeApi('192.168.8.13', 3)

def allOn():
    backlight.turn_on()
    roomled.turn_on()
    mainlamp.turn_on()

def allOff():
    backlight.turn_off()
    roomled.turn_off()
    mainlamp.turn_off()

allOn()

backlight.get