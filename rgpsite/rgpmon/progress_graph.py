from .models import *
import json

svg_lmargin = 100
svg_rmargin = 100
svg_height = 450
svg_width = 900
bwidth = 45


def create_otag(name, escargs={}, **args):
    ans = ["<" + name]
    for a in args:
        ans.append(a + '="' + str(args[a]) + '"')
    for a in escargs:
        ans.append(a + '="' + str(escargs[a]) + '"')
    return " ".join(ans) + ">"


def create_ctag(name):
    return "</" + name + ">"


def create_tag(name, escargs={}, **args):
    ans = ["<" + name]
    for a in args:
        ans.append(a + '="' + str(args[a]) + '"')
    for a in escargs:
        ans.append(a + '="' + str(escargs[a]) + '"')
    ans.append("/>")
    return " ".join(ans)


class RatingPoint:
    def __init__(self, elo, tournament, change):
        self.elo = elo
        self.tournament = tournament
        self.change = change

    def event_id(self):
        return str(self.elo) + str(self.tournament)
        
    def create_tag(self):
        if self.tournament is not None:
            cclass = "clickable"
        else:
            cclass = "notclickable"
        return create_tag("circle", cx=self.x + svg_lmargin, cy=self.y, r=3,
                          fill="white", stroke="orange",
                          onClick="rpClick(event)",
                          escargs={ "stroke-width": 2, "id": self.event_id(),
                                    "class": cclass })
        

class RatingColorRange:
    def __init__(self, color, begin, end):
        self.color = color
        self.begin = begin
        self.end = end

    def intersects(self, db_elo, ub_elo):
        return not (self.end <= db_elo or ub_elo <= self.begin)


    
class ColorBox:
    def __init__(self, color, h_low, h_high):
        self.color = color
        self.h_low = h_low
        self.h_high = h_high

    def create_tag(self):
        return create_tag("rect", x=svg_lmargin, y=self.h_low,
                          height=self.h_high - self.h_low + 1, width=svg_width,
                          fill=self.color, opacity=0.4)


class RatingColorBound:
    def __init__(self, text, h):
        self.text = str(text)
        self.h = h

    def create_tag(self):
        return create_otag("text", x=svg_lmargin - bwidth, y=self.h, fill="black", escargs={"font-size": "16px"} ) + \
               self.text + create_ctag("text")

    
class HistoryGraph:
    def __init__(self):
        self.history = []
        self.rating_color_ranges = [
            RatingColorRange("red", 1600, 2 ** 31 - 1),
            RatingColorRange("yellow", 1450, 1599),
            RatingColorRange("blue", 1300, 1449),
            RatingColorRange("green", 1000, 1299),
            RatingColorRange("gray", -2**31, 999),
        ]
        self.color_boxes = []
        self.rating_color_bounds = []


        
def get_history(change_list):
    l = list(Launch.objects.all())[0]
    with open(l.launch_cfg, 'r') as f:
        cfg = json.load(f)
    start_elo = cfg["start_elo"]

    cur_elo = start_elo
    ans = HistoryGraph()
    ans.history = [ RatingPoint(cur_elo, None, None) ]

    for c in change_list:
        cur_elo += c.elo_change
        ans.history.append(RatingPoint(cur_elo, c.tournament, c))

    min_elo = min(ans.history, key=lambda x: x.elo).elo
    max_elo = max(ans.history, key=lambda x: x.elo).elo

    elo_dist = max(16, max_elo - min_elo)
    db_elo = min_elo - elo_dist // 8
    ub_elo = max_elo + elo_dist // 8

    dst = ub_elo - db_elo
    height_coef = svg_height / dst
    width_coef = svg_width / (len(ans.history) - 1)

    for i in range(len(ans.history)):
        ans.history[i].x = width_coef * i
        ans.history[i].y = svg_height - (height_coef * (ans.history[i].elo - db_elo))

    ans.rating_color_bounds.append(RatingColorBound(ub_elo, 15))
        
    for rcr in ans.rating_color_ranges:
        if rcr.intersects(db_elo, ub_elo):
            low = max(0, svg_height - (height_coef * (rcr.end - db_elo)))
            high = min(svg_height, svg_height - (height_coef * (rcr.begin - db_elo)))
            ans.color_boxes.append(ColorBox(rcr.color, low, high))
            if high > 30 and (high == svg_height or high < svg_height - 15):
                ans.rating_color_bounds.append(RatingColorBound(max(rcr.begin, db_elo), high))
        
    return ans    


class PopUp:
    def __init__(self, hpt):
        self.hpt = hpt
        self.idn = "pu" + hpt.event_id()

    def text(self):
        rch = str(self.hpt.change.elo_change)
        if rch[0] != '-':
            rch = '+' + rch
        ans = "=" + str(self.hpt.elo) + " (" + rch + ")"
        ans += create_tag("br")
        ans += "Rank: " + str(self.hpt.change.place)
        ans += create_tag("br")
        ans += str(self.hpt.tournament)
        return ans
        
    def create_tags(self):
        ans = []
        ans.append(create_otag("foreignobject", x=self.hpt.x + svg_lmargin, y=self.hpt.y, width=200, height=80))
        ans.append(create_otag("div", escargs={ "class": "infod", "id": self.idn }))
        ans.append(create_otag("span", escargs={ "class": "infot" }))
        ans.append(self.text())
        ans.append(create_ctag("span"))
        ans.append(create_ctag("div"))
        ans.append(create_ctag("foreignobject"))
        return ans
        
    
def draw_progress_graph(change_list):
    lines = []

    # beginning of image
    lines.append(create_otag("svg", height=svg_height, width=svg_width + svg_lmargin + svg_rmargin))

    # bounding box
    lines.append(create_tag("rect", x=svg_lmargin, y=0, height=svg_height, width=svg_width,
                            stroke="black", fill="white",
                            escargs={ "stroke-width": 1 }))

    hg = get_history(change_list)
    # color boxes
    for cb in hg.color_boxes:
        lines.append(cb.create_tag())

    # color bounds
    for rcb in hg.rating_color_bounds:
        lines.append(rcb.create_tag())
    
    # lines between rating points
    for p, q in zip(hg.history[:-1], hg.history[1:]):
        lines.append(create_tag("line", x1=p.x + svg_lmargin, y1=p.y,
                                x2=q.x + svg_lmargin, y2=q.y,
                                stroke="orange",
                                escargs={ "stroke-width": 2 }))
    # rating points
    for p in hg.history:
        lines.append(p.create_tag())

    # popups
    for p in hg.history[1:]:
        a = PopUp(p)
        l = a.create_tags()
        for line in l:
            lines.append(line)
        
        
    # end of image
    lines.append(create_ctag("svg"))

    
    return "\n".join(lines)
    
    
