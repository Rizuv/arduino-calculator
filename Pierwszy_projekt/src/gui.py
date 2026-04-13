import serial
import pygame as py


try:
    data = serial.Serial("COM5", 9600, timeout=0.1)
except Exception as e:
    print(f"Błąd seriala: {e}")
    exit()

py.init()


WIDTH, HEIGHT = 800, 800
FPS = 60

screen = py.display.set_mode((WIDTH, HEIGHT))
timer = py.time.Clock()
font = py.font.Font("freesansbold.ttf", 30)

running = True

state = {
    "num1": "",
    "num2": "",
    "second_number": False,
    "operator": None,
    "result": "...",
}


data_string = []


def calculate():
    if not state["num1"] or not state["num2"] or not state["operator"]:
        return ""
    try:
        num1, num2 = float(state["num1"]), float(state["num2"])
        operator = state["operator"]
        if operator == "+":
            return num1 + num2
        if operator == "-":
            return num1 - num2
        if operator == "/":
            return num1 / num2 if num2 != 0 else "Dzielenie przez 0!"
        if operator == "*":
            return num1 * num2
    except:
        return "..."


while running:
    for event in py.event.get():
        if event.type == py.QUIT:
            running = False

    if data.in_waiting > 0:
        input = data.readline().decode("utf-8").strip()

        if input:
            if input in ["+", "-", "/", "*"] and len(state["num1"]) in range(1, 15):
                state["operator"] = input
                state["second_number"] = True
                state["result"] = calculate()
            elif input.isdigit():
                if state["second_number"] is False and len(state["num1"]) <= 15:
                    if not state["num1"].isdigit():
                        state["num1"] = ""
                    state["num1"] += input
                elif len(state["num2"]) <= 15 and state["second_number"] is True:
                    if not state["num2"].isdigit():
                        state["num2"] = ""
                    state["num2"] += input
                state["result"] = calculate()

    screen.fill((20, 20, 20))

    lines = [
        f"Liczba 1: {state['num1']}",
        f"Dzialanie: {state['operator'] if state['operator'] else '...'}",
        f"Liczba 2: {state['num2']}",
        f"Wynik: {state['result']}",
    ]

    for i, line in enumerate(lines):
        txt = font.render(line, True, "white")
        screen.blit(txt, (50, 50 + i * 50))

    py.display.flip()
    timer.tick(FPS)

data.close()
py.quit()
