from django.shortcuts import render
from django.http import HttpResponse
from django.views import generic
from django.utils.safestring import mark_safe

from .models import *
from .progress_graph import draw_progress_graph

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
    

def player_info(request, player):
    template_name='rgpmon/player.html'

    p = Player.objects.filter(name=player)
    change_list = TournamentResult.objects.filter(player=p)

    graph = draw_progress_graph(change_list)
    
    return render(request, template_name, { 'change_list': change_list,
                                            'progress_graph_svg': mark_safe(graph) })
