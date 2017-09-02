from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^$', views.PlayerListView.as_view(), name='index'),
    url(r'^alivers$', views.AlivePlayerListView.as_view(), name='alivers'),
    url(r'^population/(?P<population>.+)/alivers', views.population_alivers, name='population_alivers'),
]
