from copy import deepcopy
import json
from random import random
from flask import Flask, jsonify, make_response, request, send_from_directory
from flask import send_file
from flask_cors import CORS
import threading
import asyncio

import asyncio
import websockets
import threading
from websockets import exceptions
from websockets import server
import logging
from typing import Dict, List, Tuple

from ru_final_project.PythonParser.NewFolderStruct.Parser.ASTType import Board
from ru_final_project.PythonParser.NewFolderStruct.Parser.RegexParser.ASTType import Letter
from ..PythonParser.NewFolderStruct.GameEngine import GameEngine, Move

HOST_IP = 'localhost'
HOST_DOMAIN = 'localhost'
DEBUG_MODE = True

BROWSER_CODE_ABS_PATH = 'C:/Users/gudmu/Desktop/Lokaverkefni/lokaverkefniCode/ru_final_project/BrowserAgent/BrowserCode/'
class AgentService:
    async def handleClient(self, websocket: server.WebSocketServerProtocol):
        self.websocketClient: server.WebSocketServerProtocol = websocket
        while True:
            try:
                self.message = await websocket.recv()
            except exceptions.ConnectionClosed:
                del websocket
                break

    def __init__(self, port) -> None:
        hostDomain = HOST_DOMAIN
        self.ip = HOST_IP
        self.port = port
        self.debugMode = DEBUG_MODE

        websocketPort = 3333 + int(random()*1000)
        self.websocketClient = None
        async def webSocketMain():
            async with websockets.serve(self.handleClient, self.ip, websocketPort):
                await asyncio.Future()

        webSocketThread = threading.Thread(target=lambda: asyncio.run(webSocketMain()))
        webSocketThread.daemon = True
        webSocketThread.start()

        self.app = Flask(__name__)
        @self.app.route('/')
        def entryPoint():
            fileStr = "".join(open(BROWSER_CODE_ABS_PATH +'main.html').readlines())
            fileStr = fileStr.replace('REPLACE_WITH_WEBSOCKET_UPDATER_URL', f'{hostDomain}:{websocketPort}')
            fileStr = fileStr.replace('REPLACE_WITH_BACKEND_URL', f'{hostDomain}:{port}')
            return fileStr

        @self.app.route('/chess-board/moves', methods=['POST'])
        def chessBoardMovesHanlder():
            response_body = dict()
            if request.method == 'POST':
                moveIndex = int(request.form.get('moveIndex'))
                moves = self.gameEngine.getPlayerMoves()
                self.move = moves[moveIndex] #Storing the move
                tempEngine = deepcopy(self.gameEngine)
                tempEngine.playMove(self.move)
                response_body['gameState'] = json.loads(gameEngineToJSON(tempEngine))
                return make_response(jsonify(response_body), 200)


        @self.app.route('/<path:filename>')
        def general(filename):
            if len(filename) == 0:
                return ">:["

            for invalid_substrings in ['../']:
                if invalid_substrings in filename:
                    return ">:["

            if filename[0] == '/':
                return ">:["

            if 'app.py' in filename:
                return ">:["

            file_extension = filename.split('.')[-1]
            if file_extension in ('jpg', 'png'):
                return send_file(BROWSER_CODE_ABS_PATH+filename, mimetype='image/gif')
            elif file_extension in ('ico'):
                return send_file(BROWSER_CODE_ABS_PATH+filename, mimetype='image/vnd.microsoft.icon')
            elif file_extension in ('m4a'):
                return send_file(
                    BROWSER_CODE_ABS_PATH+filename, 
                    mimetype="audio/m4a", 
                    as_attachment=True, 
                    attachment_filename="test.wav")
            else:
                return send_from_directory(BROWSER_CODE_ABS_PATH, './', filename)

        if self.debugMode:
            webSocketThread = threading.Thread(target=lambda: self.app.run(host=self.ip, port=self.port, debug=True, use_reloader=False)) #ssl_context='adhoc'
            webSocketThread.daemon = True
            webSocketThread.start()
        else:
            from gevent.pywsgi import WSGIServer
            http_server = WSGIServer(('', self.port), self.app)
            http_server.start()


    def getMove(self, gameEngine: GameEngine):
        self.gameEngine: GameEngine = gameEngine
        while self.websocketClient is None: #Wait client to connect
            pass
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        self.move: Move = None
        tasks = [self.websocketClient.send(gameEngineToJSON(gameEngine))]
        loop.run_until_complete(asyncio.wait(tasks))
        loop.close()
        while self.move is None: #Wait for move to come in
            pass
        return self.move


def gameEngineToJSON(gameEngine: GameEngine):
    board = [[piece.name if piece is not None else 'None' for piece in line] for line in gameEngine.variables["Board"]]
    moves = '[' + ','.join([gameEngineMoveToJSON(move) for move in gameEngine.getPlayerMoves()]) + ']'
    message = f'{{"board":  {str(board)}, "moves": {moves}}}'
    return simpleObjToJson(message)

def gameEngineMoveToJSON(move: Move):
    letters = '[' + ','.join([gameEngineLetterToJSON(letter) for letter in move.letterArr])+ ']'
    return simpleObjToJson(f'{{"start":  {str(move.startPos)}, "letters": {letters}}}')

def gameEngineLetterToJSON(letter: Letter):
    return simpleObjToJson({'dx': letter.dx, 'dy':letter.dy, 'effect': str(letter.effect).replace('(','').replace(')','')})


def simpleObjToJson(simpleObj):
    # simpleObj array[simpleObj], tuple[simpleObj], string or int
    replaceInfo = {
        '(': '[',
        ')': ']',
        "'": '"'
    }

    returnStr = str(simpleObj)
    for key in replaceInfo:
        returnStr = returnStr.replace(key, replaceInfo[key])
    return returnStr




