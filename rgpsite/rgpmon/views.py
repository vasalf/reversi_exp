from django.shortcuts import render
from django.http import HttpResponse
from django.views import generic

from .models import Launch, Player

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
