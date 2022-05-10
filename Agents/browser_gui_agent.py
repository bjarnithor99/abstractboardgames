#!/usr/bin/env python3

from .agent import Agent
from .BrowserAgent.AgentService import AgentService


# An agent to play games in a browser GUI.
@Agent.register
class BrowserGUIAgent(Agent):
    def __init__(self, port):
        self.agent_service = AgentService(port)

    def get_move(self, env):
        return self.agent_service.getMove(env)
