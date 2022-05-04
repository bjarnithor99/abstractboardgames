class ChessService {
	static baseURL =`http://${BACKEND_URL}`;
	static async playMove(moveIndex) {
		try {
			const response = await axios.post(`${ChessService.baseURL}/chess-board/moves`, {'moveIndex': moveIndex});
			return response.data['gameState']
		} catch (error) {
			console.error('playMove err', error);
		}
	};
}

export default ChessService;