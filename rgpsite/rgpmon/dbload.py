import json
from .models import *


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
        except KeyError as err:
            raise DbLoadError("KeyError: {}".format(err))
        

def load():
    if Launch.objects.count() != 1:
        raise DbLoadError("Wrong amount of Launch objects in the db")

    launch = list(Launch.objects.all())[0]
    cfg = LaunchConfig(launch.launch_cfg)

    print(cfg.workdir)
    print(cfg.ntournaments)
    print(cfg.tournament_size)
    print(cfg.start_elo)
    for p in cfg.populations:
        print(p.name)
