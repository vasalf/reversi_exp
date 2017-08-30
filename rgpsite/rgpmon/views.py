from django.shortcuts import render
from django.http import HttpResponse
from django.views import generic

from .models import Player

# Create your views here.

class PlayerListView(generic.ListView):
    template_name='rgpmon/player_list.html'
    context_object_name='player_list'

    def get_queryset(self):
        return Player.objects.all()

