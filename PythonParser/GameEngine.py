from RegexParser.Parser import Parser, lex
from RegexParser.StateMachineGenerator2 import Generator, StateMachine
from RegexParser.NfaToDfa import NfaToDfaConverter
from copy import deepcopy


Letter = tuple[int,int,str]
Position = tuple[int,int]
Move = tuple[Position,Position]
def consolidateMove(letterArr: list[Letter]) -> Position:
	x, y = 0, 0
	for letter in letterArr:
		dx, dy, pre = letter
		x+=dx; y+=dy
	return (x, y)

def createSM(description: str) -> StateMachine:
	tokens = lex(description)
	abs = Parser(tokens).parse()
	nfa = Generator(abs).createStatMachine()
	dfa = NfaToDfaConverter(nfa).createDFA()
	return dfa


def evaluatePre(pre: str, pos, board):
	x, y = pos
	inRange = (0<=x<=7) and (0<=y<=7)
	returnValue =  inRange and {
		'enemy': board[x][y] is not None,
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
			if not evaluatePre(pre, (x+dx, y+dy), board):
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
		dx, dy = consolidateMove(validMoves[i])
		validMoves[i] = (pos, (x+dx, y+dy))
	return validMoves


def main():
	fileName = 'chess.game'
	gameDescription = ''
	with open(fileName) as f:
		gameDescription = ''.join(f.readlines())
		gameDescription = gameDescription.split('\n')

	players = []
	n = int(gameDescription[0])
	for i in range(n):
		playerDescription = gameDescription[i+1].replace(' ', '')
		players.append(playerDescription)
	gameDescription = gameDescription[n+1:]

	n = int(gameDescription[0])
	pieces = dict()
	for i in range(n):
		pieceDescription = gameDescription[i+1].replace(' ', '')
		pieceName, SMDescription = pieceDescription.split(':=')
		pieces[pieceName] = createSM(SMDescription)
	gameDescription = gameDescription[n+1:]
	x, y = tuple(map(int, gameDescription[0].split(' ')))
	board = [[0]*y for i in range(x)]
	for iy in range(y):
		line = gameDescription[iy+1].split(' ')
		for ix in range(x):
			pieceName = line[ix]
			if pieceName == 'None':
				board[ix][y-iy-1] = None
			else:
				temp = pieceName.split('(')
				pieceName, playerName = temp[0], temp[1][:-1]
				board[ix][y-iy-1] = pieces[pieceName]
				board[ix][y-iy-1].name = pieceName


	print(players)
	for iy in range(y):
		for ix in range(x):
			tile = board[ix][iy]
			if tile is None:
				continue
			#print(tile)
			#print(len(calcMoves(board, (ix, iy), tile)))
			print(tile.name)
			for move in calcMoves(board, (ix, iy), tile):
				print('\t', move)

		



main()
