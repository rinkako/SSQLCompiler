All: ssql
ssql: IProgram.o
	g++ -o ssql ./dst/IProgram.o ./dst/IBridge.o ./dst/IDatabase.o ./dst/IPile.o ./dst/ISyntaxParser.o ./dst/ILexicalAnalyzer.o ./dst/ITypeContainer.o -std=c++11
IProgram.o: ./src/IProgram.cpp IBridge.o
	g++ -o ./dst/IProgram.o -c ./src/IProgram.cpp -std=c++11
IBridge.o: ./src/IBridge.cpp IDatabase.o IPile.o
	g++ -o ./dst/IBridge.o -c ./src/IBridge.cpp -std=c++11
IDatabase.o: ./src/IDatabase.cpp ITypeContainer.o
	g++ -o ./dst/IDatabase.o -c ./src/IDatabase.cpp -std=c++11
IPile.o: ./src/IPile.cpp ISyntaxParser.o
	g++ -o ./dst/IPile.o -c ./src/IPile.cpp -std=c++11
ISyntaxParser.o: ./src/ISyntaxParser.cpp ILexicalAnalyzer.o
	g++ -o ./dst/ISyntaxParser.o -c ./src/ISyntaxParser.cpp -std=c++11
ILexicalAnalyzer.o: ./src/ILexicalAnalyzer.cpp ITypeContainer.o
	g++ -o ./dst/ILexicalAnalyzer.o -c ./src/ILexicalAnalyzer.cpp -std=c++11
ITypeContainer.o: ./src/ITypeContainer.cpp
	g++ -o ./dst/ITypeContainer.o -c ./src/ITypeContainer.cpp -std=c++11
clean:
	@rm -f ./dst/*.o ssql
