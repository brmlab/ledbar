import sys, time, socket, string, threading, httplib

from ledbar import Ledbar

WIDTH = 4
HEIGHT = 5
PIXELS = 20
LETTER_WIDTH = 4
HOST = "irc.freenode.net"
PORT = 6667
NICK = "Ledbar"
IDENT = "Ledbar"
REALNAME = "Brmlab Led display"

chars = {
  ' ': [
    [0, 0],
    [0, 0],
    [0, 0],
    [0, 0],
    [0, 0]
  ],
  'A': [
    [0, 1, 0],
    [1, 0, 1],
    [1, 1, 1],
    [1, 0, 1],
    [1, 0, 1]
  ],
  'B': [
    [1, 1, 0],
    [1, 0, 1],
    [1, 1, 0],
    [1, 0, 1],
    [1, 1, 0]
  ],
  'C': [
    [0, 1, 0],
    [1, 0, 1],
    [1, 0, 0],
    [1, 0, 1],
    [0, 1, 0]
  ],
  'D': [
    [1, 1, 0],
    [1, 0, 1],
    [1, 0, 1],
    [1, 0, 1],
    [1, 1, 0]
  ],
  'E': [
    [1, 1, 1],
    [1, 0, 0],
    [1, 1, 0],
    [1, 0, 0],
    [1, 1, 1]
  ],
  'F': [
    [1, 1, 1],
    [1, 0, 0],
    [1, 1, 0],
    [1, 0, 0],
    [1, 0, 0]
  ],
  'G': [
    [0, 1, 1, 0],
    [1, 0, 0, 0],
    [1, 0, 1, 1],
    [1, 0, 0, 1],
    [0, 1, 1, 0]
  ],
  'H': [
    [1, 0, 1],
    [1, 0, 1],
    [1, 1, 1],
    [1, 0, 1],
    [1, 0, 1]
  ],
  'I': [
    [1, 1, 1],
    [0, 1, 0],
    [0, 1, 0],
    [0, 1, 0],
    [1, 1, 1]
  ],
  'J': [
    [0, 1, 1],
    [0, 0, 1],
    [0, 0, 1],
    [1, 0, 1],
    [0, 1, 0]
  ],
  'K': [
    [1, 0, 1],
    [1, 0, 1],
    [1, 1, 0],
    [1, 0, 1],
    [1, 0, 1]
  ],
  'L': [
    [1, 0, 0],
    [1, 0, 0],
    [1, 0, 0],
    [1, 0, 0],
    [1, 1, 1]
  ],
  'M': [
    [1, 0, 0, 0, 1],
    [1, 1, 0, 1, 1],
    [1, 0, 1, 0, 1],
    [1, 0, 0, 0, 1],
    [1, 0, 0, 0, 1]
  ],
  'N': [
    [1, 0, 0, 1],
    [1, 1, 0, 1],
    [1, 0, 1, 1],
    [1, 0, 0, 1],
    [1, 0, 0, 1]
  ],
  'O': [
    [0, 1, 0],
    [1, 0, 1],
    [1, 0, 1],
    [1, 0, 1],
    [0, 1, 0]
  ],
  'P': [
    [1, 1, 0],
    [1, 0, 1],
    [1, 1, 0],
    [1, 0, 0],
    [1, 0, 0]
  ],
  'Q': [
    [0, 1, 1, 0],
    [1, 0, 0, 1],
    [1, 0, 0, 1],
    [1, 0, 1, 0],
    [0, 1, 0, 1]
  ],
  'R': [
    [1, 1, 0],
    [1, 0, 1],
    [1, 1, 0],
    [1, 0, 1],
    [1, 0, 1]
  ],
  'S': [
    [0, 1, 1],
    [1, 0, 0],
    [0, 1, 0],
    [0, 0, 1],
    [1, 1, 0]
  ],
  'T': [
    [1, 1, 1],
    [0, 1, 0],
    [0, 1, 0],
    [0, 1, 0],
    [0, 1, 0]
  ],
  'U': [
    [1, 0, 1],
    [1, 0, 1],
    [1, 0, 1],
    [1, 0, 1],
    [1, 1, 1]
  ],
  'V': [
    [1, 0, 1],
    [1, 0, 1],
    [1, 0, 1],
    [1, 0, 1],
    [0, 1, 0]
  ],
  'W': [
    [1, 0, 0, 0, 1],
    [1, 0, 1, 0, 1],
    [1, 0, 1, 0, 1],
    [1, 0, 1, 0, 1],
    [1, 1, 0, 1, 0]
  ],
  'X': [
    [1, 0, 1],
    [1, 0, 1],
    [0, 1, 0],
    [1, 0, 1],
    [1, 0, 1]
  ],
  'Y': [
    [1, 0, 1],
    [1, 0, 1],
    [0, 1, 0],
    [0, 1, 0],
    [0, 1, 0]
  ],
  'Z': [
    [1, 1, 1],
    [0, 0, 1],
    [0, 1, 0],
    [1, 0, 0],
    [1, 1, 1]
  ],
  '!': [
    [1],
    [1],
    [1],
    [0],
    [1]
  ],
  'a': [
    [1, 1, 0],
    [0, 0, 1],
    [0, 1, 1],
    [1, 0, 1],
    [0, 1, 0]
  ],
  'b': [
    [1, 0, 0],
    [1, 0, 0],
    [1, 1, 0],
    [1, 0, 1],
    [1, 1, 0]
  ],
  'c': [
    [0, 0, 0],
    [0, 1, 1],
    [1, 0, 0],
    [1, 0, 0],
    [0, 1, 1]
  ],
  'd': [
    [0, 0, 1],
    [0, 0, 1],
    [0, 1, 1],
    [1, 0, 1],
    [0, 1, 1]
  ],
  'e': [
    [0, 1, 0],
    [1, 0, 1],
    [1, 1, 1],
    [1, 0, 0],
    [0, 1, 0]
  ],
  'f': [
    [0, 1, 1],
    [1, 0, 0],
    [1, 1, 0],
    [1, 0, 0],
    [1, 0, 0]
  ],
  'g': [
    [0, 1, 1],
    [1, 0, 1],
    [0, 1, 1],
    [0, 0, 1],
    [1, 1, 0]
  ],
  'h': [
    [1, 0, 0],
    [1, 0, 0],
    [1, 1, 0],
    [1, 0, 1],
    [1, 0, 1]
  ],
  'i': [
    [0],
    [1],
    [0],
    [1],
    [1]
  ],
  'j': [
    [0, 0, 1],
    [0, 0, 0],
    [0, 0, 1],
    [1, 0, 1],
    [0, 1, 0]
  ],
  'k': [
    [1, 0, 0],
    [1, 0, 0],
    [1, 0, 1],
    [1, 1, 0],
    [1, 0, 1]
  ],
  'l': [
    [1, 1],
    [0, 1],
    [0, 1],
    [0, 1],
    [0, 1]
  ],
  'm': [
    [0, 0, 0, 0, 0],
    [0, 1, 0, 1, 1],
    [1, 0, 1, 0, 1],
    [1, 0, 1, 0, 1],
    [1, 0, 1, 0, 1]
  ],
  'n': [
    [0, 0, 0],
    [1, 1, 0],
    [1, 0, 1],
    [1, 0, 1],
    [1, 0, 1]
  ],
  'o': [
    [0, 0, 0],
    [0, 1, 0],
    [1, 0, 1],
    [1, 0, 1],
    [0, 1, 0]
  ],
  'p': [
    [0, 0, 0],
    [1, 1, 0],
    [1, 0, 1],
    [1, 1, 0],
    [1, 0, 0]
  ],
  'q': [
    [0, 0, 0],
    [0, 1, 1],
    [1, 0, 1],
    [0, 1, 1],
    [0, 0, 1]
  ],
  'r': [
    [0, 0],
    [0, 1],
    [1, 0],
    [1, 0],
    [1, 0]
  ],
  's': [
    [0, 0, 0],
    [0, 1, 1],
    [1, 1, 0],
    [0, 0, 1],
    [1, 1, 0]
  ],
  't': [
    [1, 0],
    [1, 1],
    [1, 0],
    [1, 0],
    [0, 1]
  ],
  'u': [
    [0, 0, 0],
    [1, 0, 1],
    [1, 0, 1],
    [1, 0, 1],
    [1, 1, 1]
  ],
  'v': [
    [0, 0, 0],
    [1, 0, 1],
    [1, 0, 1],
    [1, 0, 1],
    [0, 1, 0]
  ],
  'w': [
    [0, 0, 0, 0, 0],
    [1, 0, 0, 0, 1],
    [1, 0, 1, 0, 1],
    [1, 0, 1, 0, 1],
    [0, 1, 0, 1, 0]
  ],
  'x': [
    [0, 0, 0],
    [1, 0, 1],
    [0, 1, 0],
    [1, 0, 1],
    [1, 0, 1]
  ],
  'y': [
    [0, 0, 0],
    [1, 0, 1],
    [0, 1, 1],
    [0, 0, 1],
    [0, 0, 1]
  ],
  'z': [
    [0, 0, 0],
    [1, 1, 1],
    [0, 1, 0],
    [1, 0, 0],
    [1, 1, 1]
  ],
  '0': [
    [1, 1, 1],
    [1, 0, 1],
    [1, 0, 1],
    [1, 0, 1],
    [1, 1, 1]
  ],
  '1': [
    [0, 0, 1],
    [0, 1, 1],
    [1, 0, 1],
    [0, 0, 1],
    [0, 0, 1]
  ],
  '2': [
    [1, 1, 0],
    [0, 0, 1],
    [0, 1, 0],
    [1, 0, 0],
    [1, 1, 1]
  ],
  '3': [
    [1, 1, 0],
    [0, 0, 1],
    [0, 1, 0],
    [0, 0, 1],
    [1, 1, 0]
  ],
  '4': [
    [1, 0, 1],
    [1, 0, 1],
    [1, 1, 1],
    [0, 0, 1],
    [0, 0, 1]
  ],
  '5': [
    [1, 1, 1],
    [1, 0, 0],
    [1, 1, 0],
    [0, 0, 1],
    [1, 1, 0]
  ],
  '6': [
    [0, 1, 1],
    [1, 0, 0],
    [1, 1, 0],
    [1, 0, 1],
    [0, 1, 0]
  ],
  '7': [
    [1, 1, 1],
    [0, 0, 1],
    [0, 1, 1],
    [0, 0, 1],
    [0, 0, 1]
  ],
  '8': [
    [0, 1, 0],
    [1, 0, 1],
    [0, 1, 0],
    [1, 0, 1],
    [0, 1, 0]
  ],
  '9': [
    [0, 1, 0],
    [1, 0, 1],
    [0, 1, 1],
    [0, 0, 1],
    [1, 1, 0]
  ],
  ',': [
    [0],
    [0],
    [0],
    [1],
    [1]
  ],
  '.': [
    [0],
    [0],
    [0],
    [0],
    [1]
  ],
  ':': [
    [0],
    [1],
    [0],
    [1],
    [0]
  ],
  '=': [
    [0, 0, 0],
    [1, 1, 1],
    [0, 0, 0],
    [1, 1, 1],
    [0, 0, 0]
  ],
  '|': [
    [1],
    [1],
    [1],
    [1],
    [1]
  ]
}

def set_pixel_2d(bar, x, y, r, g, b):
  bar.set_pixel(WIDTH - x - 1 + y*WIDTH, r, g, b)

class TextScroller:
  def __init__(self):
    self.dt = 0.12
    self.reset("Hello, world!");

  def reset(self, text):
    self.text = text
    self.idx = 0
    self.col = 0
    self.kern = 1
    self.gap = 1
    self.r = 1
    self.g = 1
    self.b = 1
    self.scroll_len = len(self.text) + self.gap

  def letter_dx(self, ltr_idx):
    return self.letter_width(ltr_idx) + self.kern

  def letter_width(self, ltr_idx):
    if ltr_idx >= len(self.text):
      return 4
    if self.text[ltr_idx] in chars:
      return len(chars[self.text[ltr_idx]][0])
    else:
      return 4

  def update(self, ledbar):
    time.sleep(self.dt)
    idx = self.idx
    col = self.col
    for x in range(WIDTH):
      char = None
      if idx < len(self.text) and self.text[idx] in chars:
        char = chars[self.text[idx]]

      display = False
      if char != None and col < self.letter_width(idx):
        display = True

      for y in range(HEIGHT):
        if display and char[y][col] == 1:
          set_pixel_2d(ledbar, x, y, self.r, self.g, self.b)
        else:
          set_pixel_2d(ledbar, x, y, 0, 0, 0)

      col = col + 1
      if col == self.letter_dx(idx):
        idx += 1
        if idx >= self.scroll_len:
          idx = 0
        col = 0

    self.col += 1
    if self.col >= self.letter_dx(self.idx):
      self.idx = (self.idx + 1) % self.scroll_len
      self.col = 0
      
    return ledbar.update()

l = Ledbar(PIXELS)
work = True
scroll = TextScroller()

class WebThread(threading.Thread):
  def __init__(self):
    self.lineNum = -1
    self.line = ""
    self.mutex = threading.Lock()
    threading.Thread.__init__(self)

  def run(self):
    while(True):
      self.loop()

  def loop(self):
    http = httplib.HTTPConnection("www.cnb.cz")
    http.request("GET", "http://www.cnb.cz/cs/financni_trhy/devizovy_trh/kurzy_devizoveho_trhu/denni_kurz.txt")
    resp = http.getresponse()
    data = resp.read()
    http.close()

    linenum = 0
    self.text = "Data z CNB.cz:"
    for line in data.split("\n"):
      linenum += 1
      if linenum < 3:
        continue

      linedata = line.split("|")
      if len(linedata) < 5:
        continue
      self.text = self.text + " " + linedata[2] + " " + linedata[3] + " = " + linedata[4] + " CZK |"

    self.lock()
    self.line = self.text
    self.lineNum += 1
    self.unlock()
    time.sleep(600)

  def lock(self):
    self.mutex.acquire()

  def unlock(self):
    self.mutex.release()

  def getLine(self):
    return self.line

  def getLineNum(self):
    return self.lineNum

web = WebThread()
web.start()
prevnum = -1
while work:
  web.lock()
  if web.getLineNum() != prevnum:
    prevnum = web.getLineNum()
    scroll.reset(web.getLine())
  web.unlock()

  work = scroll.update(l)

