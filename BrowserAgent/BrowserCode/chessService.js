class ChessService {
	static baseURL =`http://${BACKEND_URL}`;
	static async playMove(moveIndex) {
		try {
			const formData = new FormData();
			formData.append('moveIndex', moveIndex);
			console.log(formData.get('moveIndex'), moveIndex);
			const response = await axios.post(`${ChessService.baseURL}/chess-board/moves`, {'moveIndex': moveIndex});
			return response.data['gameState']
		} catch (error) {
			console.error('playMove err', error);
		}
	};
}

export default ChessService;