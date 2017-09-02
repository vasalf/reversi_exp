from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^$', views.PlayerListView.as_view(), name='index'),
    url(r'^alivers$', views.AlivePlayerListView.as_view(), name='alivers')
]
