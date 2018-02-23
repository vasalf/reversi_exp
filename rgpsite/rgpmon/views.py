from django.shortcuts import render
from django.http import HttpResponse
from django.views import generic
from django.utils.safestring import mark_safe

from .models import *
from .progress_graph import draw_progress_graph

import json

# Create your views here.

class PlayerListView(generic.ListView):
    template_name='rgpmon/player_list.html'
    context_object_name='player_list'

    def get_queryset(self):
        return Player.objects.all()


class AlivePlayerListView(PlayerListView):
    template_name='rgpmon/alivers.html'
    
    def get_queryset(self):
        l = list(Launch.objects.all())[0]
        return Player.objects.filter(g_end=l.last_generation).order_by('-elo')


def population_alivers(request, population):
    template_name='rgpmon/alivers.html'

    l = list(Launch.objects.all())[0]
    p = list(Population.objects.filter(name=population))[0]
    
    player_list = Player.objects.filter(g_end=l.last_generation,
                                        population=p).order_by('-elo')
    return render(request, template_name, { 'player_list': player_list })


def rating_color(elo):
    if elo >= 1600:
        return "red"
    elif elo >= 1450:
        return "yellow"
    elif elo >= 1300:
        return "blue"
    elif elo >= 1000:
        return "green"
    else:
        return "gray"

def rating_span(elo):
    return "<span id=\"elotext\" class=\"" + rating_color(elo) + "\">" + str(elo) + "</span>"
    

class PrettifiedChange:
    def __init__(self, tch, b_elo):
        self.tournament = str(tch.tournament)
        self.place = str(tch.place)
        if tch.elo_change > 0:
            self.elod = "+" + str(tch.elo_change)
        else:
            self.elod = str(tch.elo_change)
        self.elo_change = mark_safe(rating_span(b_elo) + " &#x2192 " + rating_span(b_elo + tch.elo_change) + " (" + self.elod + ")")

        
def get_prettified_changes(change_list):
    l = list(Launch.objects.all())[0]
    with open(l.launch_cfg, 'r') as f:
        cfg = json.load(f)
    cur_elo = cfg["start_elo"]
    lst = []
    for c in change_list:
        lst.append(PrettifiedChange(c, cur_elo))
        cur_elo += c.elo_change
    return lst


def player_info(request, player):
    template_name='rgpmon/player.html'

    p = Player.objects.filter(name=player)
    change_list = TournamentResult.objects.filter(player=p)

    graph = draw_progress_graph(change_list)

    prettified_change_list = get_prettified_changes(change_list)
    
    return render(request, template_name, { 'change_list': prettified_change_list,
                                            'progress_graph_svg': mark_safe(graph),
                                            'player_name': player })
