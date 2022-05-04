import ChessService from "./chessService.js"

window.onload = () => {
    main();
}

const IDToEmojiSymbol = {
    'None': ' ',
    'BlackPawn': '♟',
    'BlackKnight': '♞',
    'BlackBishop': '♝',
    'BlackRook': '♜',
    'BlackQueen': '♛',
    'BlackKing': '♚',
    'WhitePawn': '♙',
    'WhiteKnight': '♘',
    'WhiteBishop': '♗',
    'WhiteRook': '♖',
    'WhiteQueen': '♕',
    'WhiteKing': '♔',

    'empty': ' ',
    'bPawn': '♟',
    'bKnight': '♞',
    'bBishop': '♝',
    'bRook': '♜',
    'bQueen': '♛',
    'bKing': '♚',
    'wPawn': '♙',
    'wKnight': '♘',
    'wBishop': '♗',
    'wRook': '♖',
    'wQueen': '♕',
    'wKing': '♔'
}


function getSquareID(pos) {
    return `square:${pos.x}-${pos.y}`;
}

function createSquare(x, y, pieceID) {
    const square = document.createElement("div");
    square.id = getSquareID({x, y});
    square.className = 'chess-square ' + ((x+y)%2===0?'black-square':'white-square');

    const squareSymbol = document.createElement("div");
    squareSymbol.appendChild(document.createTextNode(pieceID));
    squareSymbol.className = 'chess-piece text-unselectable';

    const dotSymbol = document.createElement("div");
    dotSymbol.className = 'move-dot fill hide';

    [squareSymbol, dotSymbol].forEach(squareChild => square.appendChild(squareChild));
    return square;
}

function clearBoard() {
    const boardElement = document.getElementById('chess-board');
    var child = boardElement.lastElementChild; 
    while (child) {
        boardElement.removeChild(child);
        child = boardElement.lastElementChild;
    }

}

var indexQueryCame = false;

async function createChessBoard(moves, board) {
    let selectedSquare = null;
    let X, Y;X = board.length; Y = board[0].length;
    const movesBoard = new Array(X).fill(0).map(() => (new Array(Y).fill(0)).map(() => []));
    let x, y, dx, dy, effectStr;
    moves.forEach((move, i) => {
        effectStr = ''
        x = move.start[0]; y = move.start[1];
        dx = 0; dy = 0;
        move.letters.forEach(letter => {
            dx += letter.dx; dy += letter.dy;
            if(letter.effect != 'None')effectStr += letter.effect;
        });
        if(effectStr == '')effectStr='Ɛ';
        movesBoard[x][y].push({i, dPos: {dx, dy}, effect: effectStr});
    });
    console.log('updating moves', movesBoard)

    const boardElement = document.getElementById('chess-board');
    [...Array(X).keys()].forEach(x => {
        const column = document.createElement("div");
        [...Array(Y).keys()].map(y => Y-y-1).forEach(y => { 
            const square = createSquare(x, y, IDToEmojiSymbol[board[x][y]]);

            square.onclick = () => {
                if (selectedSquare) {
                    let xOld, yOld;
                    xOld = selectedSquare.x; yOld = selectedSquare.y;


                    let possibleMoves = []
                    movesBoard[xOld][yOld].forEach(async move => {
                        const {i, dPos, effect} = move;
                        const {dx, dy} = dPos;
                        if (getSquareID({x, y}) === getSquareID({x:xOld+dx, y:yOld+dy}))possibleMoves.push(move);
                    });

        const playMove = (async (move) => {
                        const {i, dPos, effect} = move;
                        // Move played
                        indexQueryCame = false;
                        let gameState = await ChessService.playMove(i);
                        board = gameState.board;
                        if (!indexQueryCame) {
                            clearBoard();
                            createChessBoard([], board);
                        }
                    });

                    if(possibleMoves.length == 1) playMove(possibleMoves[0]);
        else if (possibleMoves.length > 0){
            const optionAndEffects = possibleMoves.map(move => {
            const {i, dPos, effect} = move;
                        const {dx, dy} = dPos;
            return {'option': effect, 'effect': () => playMove(move)};
            })
            const selectionList = createSelectionList(optionAndEffects);
            square.appendChild(selectionList);
            const killSelectionList = () => {
            square.removeChild(selectionList);
            document.body.removeEventListener('click', killSelectionList, true);
            };
            document.body.addEventListener('click', killSelectionList, true); 
        }

                    document.getElementById(getSquareID(selectedSquare)).classList.remove('selected-square');
                    [...Array(X).keys()].forEach(x => [...Array(Y).keys()].forEach(y => {
                        const square = document.getElementById(getSquareID({x, y}));
                        const dot = square.querySelector('.move-dot');
                        dot.classList.add('hide');
                    }));
                }
                if (selectedSquare && (getSquareID(selectedSquare) === square.id)) {
                    selectedSquare = null;
                } else {
                    selectedSquare = {x, y}
                    square.classList.add('selected-square');

                    movesBoard[x][y].forEach(move => {
                        const {i, dPos, effect} = move;
                        const {dx, dy} = dPos;

                        const square = document.getElementById(getSquareID({x: x+dx, y: y+dy}));
                        const dot = square.querySelector('.move-dot');
                        dot.classList.remove('hide');
                    });
                }
            }
            column.appendChild(square);
        }); 
        column.className = 'chess-board-column';
        boardElement.appendChild(column);
    });
}

function createSelectionList(optionAndEffects) {
    const nav = document.createElement("nav");
    nav.classList.add('nav-list');
    const ul = document.createElement("ul");
    ul.classList.add('drop-down');
    nav.appendChild(ul);


    optionAndEffects.forEach(optionAndEffect => {
        const {option, effect} = optionAndEffect;
        let li = document.createElement("li");
        li.onclick = effect;
        ul.appendChild(li);
        let a = document.createElement("a");
        a.href = '#'
        li.appendChild(a);
        a.appendChild(document.createTextNode(option));
    });
    return nav;
};

async function main() {
    const socket = new WebSocket(`ws://${WEBSOCKET_UPDATER_URL}`);
    socket.onmessage = async (event) => {
        const response = JSON.parse(event.data);
        const moves = response.moves;
        const board = response.board;
        indexQueryCame = true;
        clearBoard();
        createChessBoard(moves, board);
    };
}
