from django.db import models

# Create your models here.


class Launch(models.Model):
    last_generation = models.IntegerField(default=-1)
    launch_cfg = models.CharField(max_length=200)

    def __str__(self):
        return "{} (ng={})".format(self.launch_cfg, self.last_generation)


class Population(models.Model):
    name = models.CharField(max_length=200)

    def __str__(self):
        return self.name

    
class Player(models.Model):
    population = models.ForeignKey(Population, on_delete=models.CASCADE)
    name = models.CharField(max_length=200)
    g_start = models.IntegerField()
    g_end = models.IntegerField()
    elo = models.IntegerField()

    def __str__(self):
        return "{} (elo={})".format(self.name, self.elo)
    

class Tournament(models.Model):
    name = models.CharField(max_length=200)
    generation = models.IntegerField()

    def __str__(self):
        return "generation{}/{}".format(self.generation, self.name)
    
    
class TournamentResult(models.Model):
    player = models.ForeignKey(Player, on_delete=models.CASCADE)
    tournament = models.ForeignKey(Tournament, on_delete=models.CASCADE)
    place = models.IntegerField()
    elo_change = models.IntegerField()

    def __str__(self):
        player_name = self.player.name
        tournament_name = str(self.tournament)
        if self.elo_change <= 0:
            return "{} at {}: {} ({})".format(player_name, tournament_name, self.place, self.elo_change)
        else:
            return "{} at {}: {} (+{})".format(player_name, tournament_name, self.place, self.elo_change)
