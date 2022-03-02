
def getDefinitionsInProgram(program: list[str]) -> dict:
    definitions = dict()
    for line in program:
        if ':=' not in line:
            continue
        definitionName, value = line.split(':=')
        definitions[definitionName] = value
    return definitions
    
class Macro:
    def __init__(self, argumentNames: list[str], macroString: str) -> None:
        self.argumentNames = argumentNames
        self.macroString = macroString

    def resolve(self, arguments: list[str]) -> str:
        resolveStr = self.macroString
        for i in range(len(self.argumentNames)):
            argumentName, argument = self.argumentNames[i], arguments[i]
            resolveStr = resolveStr.replace(argumentName, argument) # Possible collisions because of order that is replaced
        return resolveStr
            
from ast import arguments
from copy import deepcopy
from RegexParser.main import createSM, StateMachine
def resolveDefinitionsToType(definitions: dict[str, str]) -> dict:
    resolvedDefinitions = dict()
    for definitionName in definitions:
        value = definitions[definitionName]

        if '{' in value:
            value = value[1:-1]
            resolvedDefinitions[definitionName] = list(value.split(','))
            continue

        if '(' in definitionName:
            definitionName, argumentNames = definitionName.split('(')
            argumentNames = argumentNames[:-1]
            argumentNames = argumentNames.split(',')
            resolvedDefinitions[definitionName] = Macro(argumentNames, value)
            continue



        key = next((key for key in resolvedDefinitions if key in value), None)
        while key is not None:
            if isinstance(resolvedDefinitions[key], Macro):
                macro: Macro =  resolvedDefinitions[key]
                arguments = []
                keyStart = value.index(key)
                argumentStart = value[keyStart:].index('(')+1

                i = keyStart + argumentStart
                char = value[i]
                while char != ')':
                    token = ''
                    while char != ')' and char != ',':
                        token += char
                        i += 1
                        char = (value+')')[i] 
                    arguments.append(token)
                    if char == ',':
                        i += 1
                        char = (value+')')[i]
                value = value[0:keyStart] + '(' + macro.resolve(arguments)+')' + value[i+1:]
            else:
                value = value.replace(key, '('+definitions[key]+')')
            key = next((key for key in resolvedDefinitions if key in value), None)
        resolvedDefinitions[definitionName] = createSM(value)
    return resolvedDefinitions

    
Letter = tuple[int,int,str]
Position = tuple[int,int]
Move = tuple[Position,Position]


class Player:
    def __init__(self, name: str, decisionFunction) -> None:
        self.name = name
        self.decisionFunction = decisionFunction


class Game:
    def __init__(self, stateMachines: dict, boardDescription: list[str], boardSize: list[str]) -> None: #, #players: list[Player]) -> None:
        #self.players: list[Player] = players
        self.playersTurnIndex: int = 0
        self.x, self.y = tuple(map(int, boardSize))
        self.board = [[None]*self.y for i in range(self.x)]
        for iy in range(self.y):
            #line = boardDescription[iy+1].split(' ')
            for ix in range(self.x):
                pieceName = boardDescription[ix+iy*self.y]
                if pieceName == 'None':
                    self.board[ix][self.y-iy-1] = None
                else:
                    temp = pieceName.split('(')
                    pieceName, playerName = temp[0], temp[1][:-1]
                    self.board[ix][self.y-iy-1] = deepcopy(stateMachines[pieceName])
                    self.board[ix][self.y-iy-1].name = pieceName
                    self.board[ix][self.y-iy-1].player = playerName

    def consolidateMove(letterArr: list[Letter]) -> Position:
        x, y = 0, 0
        for letter in letterArr:
            dx, dy, pre = letter
            x+=dx; y+=dy
        return (x, y)

    def evaluatePre(pre: str, pos, board, playerName: str):
        x, y = pos
        inRange = (0<=x<=7) and (0<=y<=7)
        returnValue =  inRange and {
            'enemy': board[x][y] is not None and (board[x][y].player != playerName),
            'empty': board[x][y] is None,
            'true': True
        }[pre]
        return returnValue


    def calcMoves(board, pos, sm: StateMachine) -> list[Move]:
        def rec(board, pos, sm: StateMachine) -> list[list[Letter]]:
            x, y = pos
            validMoves = []
            for transition in sm.start.outTransitions:
                letter = transition.letter
                dx, dy, pre = letter if letter is not None else (0, 0, 'true')
                if not Game.evaluatePre(pre, (x+dx, y+dy), board, sm.player):
                    continue

                if transition.end.final:
                    validMoves.append([] if letter is None else [letter])

                thingOnTile = board[x+dx][y+dy]
                board[x+dx][y+dy] = sm
                board[x][y] = None
                oldStart = sm.start
                sm.start = transition.end

                transitionMoves = rec(board, (x+dx, y+dy), sm)
                if letter is None:
                    validMoves.extend(transitionMoves)
                else:
                    validMoves.extend([[letter, *move] for move in transitionMoves])

                sm.start = oldStart
                board[x+dx][y+dy] = thingOnTile
                board[x][y] = sm
            return validMoves

        x, y = pos
        validMoves = rec(board, pos, sm)
        for i in range(len(validMoves)):
            dx, dy = Game.consolidateMove(validMoves[i])
            validMoves[i] = (pos, (x+dx, y+dy))
        return validMoves

    def getPlayerMoves(self, playersName: str):
        moves = []
        for iy in range(self.y):
            for ix in range(self.x):
                tile = self.board[ix][iy]
                if tile is None:
                    continue
                if tile.player != playersName:
                    continue
                print(tile.player ,tile.name, f'on x,y: ({ix},{iy}) moves:')
                for move in Game.calcMoves(self.board, (ix, iy), tile):
                    print('\t', move)

                #moves.extend(Game.calcMoves(self.board, (ix, iy), tile))
        #return moves

    def gameOver(self):
        return len[(1 for tile in self.board if tile.name == 'King')] == 1


    def playMove(self, move):
        (x0,y0), (x1,y1) = move
        self.board[x1][y1] = self.board[x0][y0]
        self.board[x0][y0] = None


    def runGame(self):
        while not self.gameOver():
            player = self.players[self.playersTurnIndex]
            moves = self.getPlayerMoves(player.name)
            move = player.decisionFunction(moves, self.board)
            self.playMove(move)
            self.playersTurnIndex = (self.playersTurnIndex+1)%len(self.playersTurnIndex)
        print('Game over.')

fileName = 'chess3.game'
program = ''
with open(fileName) as f:
    program = ''.join(list(map(lambda line: line[:-1].replace(' ',''), f.readlines()))).split(';')

#print(program)
definitions = getDefinitionsInProgram(program)
resolvedDefinitions = resolveDefinitionsToType(definitions)
#print(resolvedDefinitions)

from random import choice
def randomAIDecisionFunction(moves, boardState):
    return choice(moves)

#players = list(map(lambda name: Player(name, randomAIDecisionFunction), resolvedDefinitions['Players']))
game = Game(resolvedDefinitions, resolvedDefinitions['Board'], resolvedDefinitions['BoardSize'])
#print(resolvedDefinitions['Players'])
game.getPlayerMoves(resolvedDefinitions['Players'][0])
#print(resolvedDefinitions)