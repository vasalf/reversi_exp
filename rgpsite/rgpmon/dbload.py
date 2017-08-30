import json
import pathlib
from .models import *
from django.core.exceptions import ObjectDoesNotExist


class DbLoadError(Exception):
    def __init__(self, message):
        self.message = message

    def __str__(self):
        return self.message
    

class PopulationConfig:
    def __init__(self, cfg):
        self.__dict__["cfg"] = cfg

    def __getattr__(self, name):
        if name not in self.__dict__["cfg"]:
            raise AttributeError(name)
        return self.__dict__["cfg"][name]

    def __setattr__(self, name, value):
        if name not in self.cfg:
            raise AttributeError(name)
        self.__dict__["cfg"][name] = value
    
        
class LaunchConfig:
    def __init__(self, filename):
        with open(filename, 'r') as f:
            cfg = json.load(f)
        try:
            self.workdir = cfg["workdir"]
            self.ntournaments = cfg["ntournaments"]
            self.tournament_size = cfg["tournament_size"]
            self.start_elo = cfg["start_elo"]
            self.populations = [PopulationConfig(p) for p in cfg["populations"]]
            self.populations_dict = dict()
            for p in self.populations:
                self.populations_dict[p.name] = p
        except KeyError as err:
            raise DbLoadError("KeyError: {}".format(err))
    

class GenerationFiles:
    def __init__(self, cfg, generation):
        self.generation = generation
        self.workdir = pathlib.Path(cfg.workdir)
        self.generation_dir = self.workdir / "generation{0:04d}".format(generation)

    def ready_file(self):
        return self.generation_dir / 'ready'

    def populations_old_json(self):
        return self.generation_dir / 'populations_old.json'

    def populations_new_json(self):
        return self.generation_dir / 'populations_new.json'

    def strategies_dir(self):
        return self.generation_dir / 'strategies'

    def strategy_rsf(self, strategy):
        return self.strategies_dir() / '{}.rsf'.format(strategy)

    def tournaments_dir(self):
        return self.generation_dir / 'tournaments'

    def tournament_name(self, ts, tn):
        return 'TN{0}T{1}'.format(ts, tn)
    
    def tournament_json(self, ts, tn):
        return self.tournaments_dir() / '{}.json'.format(self.tournament_name(ts, tn))

    def tournament_rgf(self, ts, tn):
        return self.tournaments_dir() / '{}.rgf'.format(self.tournaments_name(ts, tn))


def is_generation_dumped(cfg, generation):
    gf = GenerationFiles(cfg, generation)
    if not gf.generation_dir.exists() \
       or not gf.generation_dir.is_dir() \
       or not gf.ready_file().exists():
        return False
    with open(str(gf.ready_file()), 'r') as f:
        if f.read() != '1':
            return False
    return True


def get_or_create_population(name):
    print("population", name)
    try:
        p = Population.objects.get(name=name)
    except ObjectDoesNotExist:
        p = Population()
        p.name = name
        p.save()
    return p


def get_or_create_player(population, plcfg, gex):
    print("player", plcfg["name"])
    try:
        p = Player.objects.get(name=plcfg["name"])
    except ObjectDoesNotExist:
        p = Player()
        p.population = population
        p.name = plcfg["name"]
        p.g_start = gex
        p.g_end = gex
    p.elo = plcfg["ratings_history"][-1]
    p.save()
    return p

    
def load_generation(cfg, generation):
    gf = GenerationFiles(cfg, generation)

    # load tournaments
    # TODO

    # load players
    with open(str(gf.populations_old_json()), 'r') as f:
        pn = json.load(f)
    for pcfg in pn["populations"]:
        p = get_or_create_population(pcfg["name"])
        for plcfg in pcfg["individuals"]:
            pl = get_or_create_player(p, plcfg, generation)
            pl.g_end = generation
            pl.save()

        
def load():
    if Launch.objects.count() != 1:
        raise DbLoadError("Wrong amount of Launch objects in the db")

    launch = list(Launch.objects.all())[0]
    cfg = LaunchConfig(launch.launch_cfg)

    while (is_generation_dumped(cfg, launch.last_generation + 1)):
        print("generation", launch.last_generation)
        load_generation(cfg, launch.last_generation + 1)
        launch.last_generation += 1
    launch.save()

    
