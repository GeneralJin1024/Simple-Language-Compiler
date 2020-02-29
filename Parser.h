#include "Scanner.h"
#include <vector>
#include <map>
#include <stack>

class Term;
class Assign;
class IfThenElse;
class Loop;
class Input;
class Output;
class FuncInvoke;
class StmtSeq;

class Parser { 
    public:
        static Scanner* S;
        static Scanner* Input;
        //static std::string inputFile;
        //std::vector<Id*> idList;
        //std::vector<Func*> funcList;
        Parser(){}
};

class Id{
    private:
        std::string idName;
        int value;
        bool isAssigned;
    public:
        Id(std::string name){
            idName = name;
            value = -1;
            isAssigned = false;
        }
        void assign(int v){
            value = v;
            isAssigned = true;
        }
        void print(){
            std::cout << idName;
        }
        int getValue(){
            return value;
        }
        std::string getIdName(){
            return idName;
        }
};

class IdList{
    private:
        int idNum;
        Id* id;
        IdList* idList;
    public:
        IdList(){
            idNum = 0;
            id = NULL;
            idList = NULL;
        }
        void parse(){
            if (Parser::S->currentToken() != ID) std::cout << "ERROR: There is an invalid ID being declared " << std::endl;
            id = new Id(Parser::S->getID());
            Parser::S->nextToken();
            if (Parser::S->currentToken() == SEMICOLON || Parser::S->currentToken() == RPAREN) return;
            Parser::S->match(COMMA);
            idList = new IdList();
            idList->parse();
        }
        void print(){
            id->print();
            if (idList != NULL){
                std::cout << ","; idList->print();
            }
        }
        void exec(){
            if (idList != NULL) idList->exec();
        }
        void argumentsCheck(){
            if (id != NULL) {
                idNum++;
            }
            if (idList != NULL) idList->argumentsCheck();
        }
        void declareIDs();
        std::string getIdNames(){
            if (id != NULL){
                return id->getIdName();
            }else{
                std::cout << "ERROR: # of ID are less than # of inputs" << std::endl;
                exit(0);
            }
        }
        Id* getID(){
            if (id != NULL){
                return id;
            }else{
                std::cout << "ERROR: IDs are not enough" << std::endl;
                exit(0);
            }
        }
        IdList* nextIDList(){ 
            return this->idList;  
        }
        int getArgumentsNum(){
            return idNum;
        }
};

class Func{
    private:
        //std::map<std::string, int>* idMap;
        std::vector<Id*> formalParm;
        IdList* arguments;
        Id* funcName;
        StmtSeq* funcBody;
    public:
        Func(){
            funcName = NULL;
            arguments = NULL;
        }
        Func(std::string name){
            funcName = new Id(name);
            arguments = NULL;
        }
        IdList* parseInvoke(){
            Parser::S->match(LPAREN);
            arguments = new IdList();
            arguments->parse();
            arguments->argumentsCheck();
            if (arguments->getArgumentsNum() != formalParm.size()){
                std::cout << "ERROR: Arguments do not match Func's parameters" << std::endl;
            }
            Parser::S->match(RPAREN);
            Parser::S->match(SEMICOLON);
            return arguments;
        }
        void getFuncBody(StmtSeq* st);
        void print(){
            arguments->print();
        }
        void exec();
        void declID(Id* id){
            for (int i = 0; i < formalParm.size(); ++i){
                if (formalParm[i]->getIdName().compare(id->getIdName()) == 0){
                    std::cout << "ERROR: Duplicated ID declared" << std::endl;
                    exit(0);
                }
            }
            formalParm.push_back(id);
            
        }
        void assignID(std::string idName, int idValue){
            for (int i = 0; i < formalParm.size(); ++i){
                if (formalParm[i]->getIdName().compare(idName) == 0){
                    formalParm[i]->assign(idValue);
                    break;
                }
            }
        }
        int loadID(std::string idName){
            for (std::vector<Id*>::iterator it = formalParm.begin(); it != formalParm.end(); ++it){
                Id* target = *it;
                if (target->getIdName().compare(idName) == 0) return target->getValue();
            }
            std::cout << "ERROR: An argument " << idName << " not declared" << std::endl;
            exit(0);
        }
        std::string getFuncName(){
            return funcName->getIdName();
        }
};

class Memory {
    private:
        static Func* activeFunc;
    public:
        static std::stack<Func*> funcStack;
        static std::vector<Func*> funcList;
        Memory(){}

        static void declID(Id* id){
            activeFunc = funcStack.top();
            activeFunc->declID(id);
        }

        static void assignID(std::string name, int value){
            activeFunc = funcStack.top();
            activeFunc->assignID(name, value);
        }

        static int LoadID(std::string name){
            activeFunc = funcStack.top();
            return activeFunc->loadID(name);
        }

        static void addFunc(Func* func){
            funcList.push_back(func);
            funcStack.push(func);
        }

        static void endFunc(){
            funcStack.pop();
        }

        static IdList* parseInvokeFunc(std::string funcName){
            bool isFind = false;
            for (int i = 0; i < funcList.size(); ++i){
                if (funcList[i]->getFuncName().compare(funcName) == 0){
                    isFind = true;
                    return funcList[i]->parseInvoke();
                }
            }
            if (!isFind){
                std::cout << "ERROR: There is no Func: "<< funcName << "exist" << std::endl;
                exit(0);
            }
        }

        static void execFunc(std::string funcName){
            for (int i = 0; i < funcList.size(); ++i){
                if (funcList[i]->getFuncName().compare(funcName) == 0){
                    funcStack.push(funcList[i]);
                    funcList[i]->exec();
                    return;
                }
            }
        }
};

void IdList::declareIDs(){
    if (id != NULL) Memory::declID(id);
    //std::cout << id->getIdName() << " declared" << std::endl;
    if (idList != NULL) idList->declareIDs();
}

class Expr{
    private:
        int altNo;  //1 = +; 2 = -;
        Term* term;
        Expr* expr;
    public:
        Expr(){
            altNo = 0;
            term = NULL;
            expr = NULL;
        }
        void parse();
        void print();
        int exec();
};

class Factor{
    private:
        int altNo;
        int constF;
        std::string id;
        Expr* expr;
    public:
        Factor(){
            altNo = 0;
            constF = -1;
            id = "";
            expr = NULL;
        }
        void parse(){
            if (Parser::S->currentToken() == CONST){
                altNo = 1;
                constF = Parser::S->getCONST();
                Parser::S->nextToken();
                return;
            }
            if (Parser::S->currentToken() == ID){
                altNo = 2;
                id = Parser::S->getID();
                //std::cout << id << std::endl;
                Memory::LoadID(id);
                Parser::S->nextToken();
                return;
            }
            if (Parser::S->currentToken() == LPAREN){
                altNo = 3;
                Parser::S->match(LPAREN);
                expr = new Expr();
                expr->parse();
                Parser::S->match(RPAREN);
                return;
            }
        }
        void print(){
            if (altNo == 1) std::cout << constF;
            if (altNo == 2) std::cout << id;
            if (altNo == 3){
                std::cout << "(";
                expr->print();
                std::cout << ")";
            }
        }
        int exec(){
            if (altNo == 1) return constF;
            if (altNo == 2) return Memory::LoadID(id);
            if (altNo == 3) return expr->exec();
            std::cout << "ERROR: No factor to be evaluated" << std::endl;
            exit(-1);
        }
};

class Term{
    private:
        Factor* factor;
        Term* term;
    public:
        Term(){
            factor = NULL;
            term = NULL;
        }
        void parse(){
            factor = new Factor();
            factor->parse();
            if (Parser::S->currentToken() == MULT){
                Parser::S->match(MULT);
                term = new Term();
                term->parse();
            }
        }
        void print(){
            factor->print();
            if (term != NULL){
                std::cout << "*";
                term->print();
            }
        }
        int exec(){
            if (term != NULL) {
                int v = term->exec();
                return factor->exec() * v;
            } else {
                return factor->exec();
            }
        }
};

void Expr::parse(){
            term = new Term();
            term->parse();
            if (Parser::S->currentToken() == ADD){
                altNo = 1;
                Parser::S->match(ADD);
                expr = new Expr();
                expr->parse();
                return;
            } else if (Parser::S->currentToken() == SUB){
                altNo = 2;
                Parser::S->match(SUB);
                expr = new Expr();
                expr->parse();
                return;
            }
        }
void Expr::print(){
            term->print();
            if (altNo == 0) return;
            if (altNo == 1) std::cout << "+";
            if (altNo == 2) std::cout << "-";
            expr->print();
        }
int Expr::exec(){
            if (altNo == 0) return term->exec();
            if (altNo == 1) {
                int v = expr->exec();
                return term->exec() + v;
            }
            if (altNo == 2) {
                int v = expr->exec();
                return term->exec() - v;
            }
        }
class Cmpr{
    private:
        int altNo;
        Expr* expr1;
        Expr* expr2;
    public:
        Cmpr(){
            altNo = 0;
            expr1 = NULL;
            expr2 = NULL;
        }
        void parse(){
            expr1 = new Expr();
            expr1->parse();
            if (Parser::S->currentToken() == EQUAL){
                altNo = 1;
                Parser::S->match(EQUAL);
            }
            if (Parser::S->currentToken() == LESS){
                altNo = 2;
                Parser::S->match(LESS);
            }
            if (Parser::S->currentToken() == LESSEQUAL){
                altNo = 3;
                Parser::S->match(LESSEQUAL);
            }
            expr2 = new Expr();
            expr2->parse();
        }
        void print(){
            expr1->print();
            if (altNo == 1) std::cout << "=";
            if (altNo == 2) std::cout << "<";
            if (altNo == 3) std::cout << "<=";
            expr2->print();
        }
        bool exec(){
            int leftV = expr1->exec();
            int rightV = expr2->exec();
            if (altNo == 1) return (leftV == rightV);
            if (altNo == 2) return (leftV < rightV);
            if (altNo == 3) return (leftV <= rightV);
        }
};

class Cond{
    private:
        int altNo; 
        Cmpr* cmpr;
        Cond* cond;
    public:
        Cond(){
            altNo = 0;
            cmpr = NULL;
            cond = NULL;
        }
        void parse(){
            if (Parser::S->currentToken() == NEGATION){
                altNo = 1;
                Parser::S->match(NEGATION);
                Parser::S->match(LPAREN);
                cond = new Cond();
                cond->parse();
                Parser::S->match(RPAREN);
                return;
            }else
            {
                cmpr = new Cmpr();
                cmpr->parse();
                altNo = 2;
                if (Parser::S->currentToken() == OR){
                    altNo = 3;
                    Parser::S->match(OR);
                    cond = new Cond();
                    cond->parse();
                }
            }
            
        }
        void print(){
            if (altNo == 1){
                std::cout << "!(";
                cond->print();
                std::cout << ")";
                return;
            }
            if (altNo == 2){
                cmpr->print();
                return;
            }
            if (altNo == 3){
                cmpr->print();
                std::cout << " or ";
                cond->print();
                return;
            }
        }
        bool exec(){
            if (altNo == 1) return !cond->exec();
            if (altNo == 2) return cmpr->exec();
            if (altNo == 3) return (cmpr->exec() || cond->exec());
        }
};

class Stmt {
    private:
        int altNo;
        Assign* s1;
        IfThenElse* s2;
        Loop* s3;
        Input* s4;
        Output* s5;
        FuncInvoke* s6;
    public:
        Stmt(){
            altNo = 0;
            s1 = NULL;
            s2 = NULL;
            s3 = NULL;
            s4 = NULL;
            s5 = NULL;
            s6 = NULL;
        }
        void parse();
        void print(int indent);
        void exec();        
};

class StmtSeq {
    private: 
        Stmt* stmt;
        StmtSeq* stmt_seq;
    public:
        StmtSeq(){
            stmt = NULL;
            stmt_seq = NULL;
        }
        void parse(){
            stmt = new Stmt();
            stmt->parse();
            if (Parser::S->currentToken() == END || Parser::S->currentToken() == ELSE || Parser::S->currentToken() == ENDIF
            || Parser::S->currentToken() == ENDWHILE || Parser::S->currentToken() == ENDFUNC) return;
            if (Parser::S->currentToken() == 28){
                std::cout << "ERROR: Missing END " << std::endl;
                exit(-1);
            }
            stmt_seq = new StmtSeq();
            stmt_seq->parse();
        }
        void print(int indent){
            stmt->print(indent);
            if (stmt_seq != NULL) stmt_seq->print(indent);
        }
        void exec(){
            stmt->exec();
            if (stmt_seq != NULL) stmt_seq->exec();
        }
};

class Assign {
    private:
        std::string idName;
        Expr* expr;
    public:
        Assign(){
            idName = "";
            expr = NULL;
        }
        void parse(){
            idName = Parser::S->getID();
            Memory::LoadID(idName);
            Parser::S->nextToken();
            Parser::S->match(ASSIGN);
            expr = new Expr();
            expr->parse();
            Parser::S->match(SEMICOLON);
        }
        void print(int indent){
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            std::cout << idName << ":=";
            expr->print();
            std::cout <<";"<< std::endl;
        }
        void exec(){
            Memory::assignID(idName, expr->exec());
        }

};

class IfThenElse {
    private:
        Cond* cond;
        StmtSeq* stmt_seq1;
        StmtSeq* stmt_seq2;
    public:
        IfThenElse(){
            cond = NULL;
            stmt_seq1 = NULL;
            stmt_seq2 = NULL;
        }
        void parse(){
            Parser::S->match(IF);
            cond = new Cond();
            cond->parse();
            Parser::S->match(THEN);
            stmt_seq1 = new StmtSeq();
            stmt_seq1->parse();
            if (Parser::S->currentToken() == ENDIF) {
                Parser::S->match(ENDIF);
                Parser::S->match(SEMICOLON);
                return;
            }
            Parser::S->match(ELSE);
            stmt_seq2 = new StmtSeq();
            stmt_seq2->parse();
            Parser::S->match(ENDIF);
            Parser::S->match(SEMICOLON);
        }
        void print(int indent){
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            std::cout << "if ";
            cond->print();
            std::cout << " then " << std::endl;
            stmt_seq1->print(indent+2);
            if (stmt_seq2 != NULL) {
                for (int i = 0; i < indent; ++i)
                    std::cout << " ";
                std::cout << "else" << std::endl;
                stmt_seq2->print(indent+2);
            }
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            std::cout << "endif;" << std::endl;
        }
        void exec(){
            if (cond->exec()){
                stmt_seq1->exec();
            }else{
                stmt_seq2->exec();
            }
        }
};

class Loop {
    private:
        Cond* cond;
        StmtSeq* stmtSeq;
    public:
        Loop(){
            cond = NULL;
            stmtSeq = NULL;
        }
        void parse(){
            Parser::S->match(WHILE);
            cond = new Cond();
            cond->parse();
            Parser::S->match(BEGIN);
            stmtSeq = new StmtSeq();
            stmtSeq->parse();
            Parser::S->match(ENDWHILE);
            Parser::S->match(SEMICOLON);
        }
        void print(int indent){
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            std::cout << "while ";
            cond->print();
            std::cout << " begin " << std::endl;
            stmtSeq->print(indent+2);
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            std::cout << "endwhile;" << std::endl;
        }
        void exec(){
            while (cond->exec()){
                stmtSeq->exec();
            }
        }

};

class Input {
    private:
        IdList* idList;
        //std::ifstream myfile;
    public:
        Input(){
            idList = NULL;
        }
        void parse(){
            Parser::S->match(INPUT);
            idList = new IdList();
            idList->parse();
            checkIDList();
            Parser::S->match(SEMICOLON);
            //std::cout << "Input done" << std::endl;
        }
        void print(int indent){
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            std::cout << "input ";
            idList->print();
            std::cout << ";" << std::endl;
        }
        void exec(){
            IdList* tempList = idList;
            while (tempList != NULL){
                if (Parser::Input->currentToken() == EOS){
                    std::cout << "ERROR: # of ID are less than # of inputs" << std::endl;
                    exit(0);
                }
                int value = 1;
                if (Parser::Input->currentToken() == SUB){
                    value *= -1;
                    Parser::Input->nextToken();
                    if (Parser::Input->currentToken() != CONST) {
                        std::cout << "Invalid Input" << std::endl;
                        exit(-1);
                    }
                    value *= Parser::Input->getCONST();
                    //std::cout << "read " << Parser::Input->getCONST() << std::endl;
                    Memory::assignID(tempList->getIdNames(), value);
                    tempList = tempList->nextIDList();
                    Parser::Input->nextToken();
                }else if(Parser::Input->currentToken() == CONST){
                    value *= Parser::Input->getCONST();
                    //std::cout << "read " << Parser::Input->getCONST() << std::endl;
                    Memory::assignID(tempList->getIdNames(), value);
                    tempList = tempList->nextIDList();
                    Parser::Input->nextToken();
                }else{
                    std::cout << "Invalid Input" << std::endl;
                    exit(-1);
                }
            }
        }
        void checkIDList(){
            IdList* tempList = idList;
            while (tempList != NULL){
                Memory::LoadID(tempList->getIdNames());
                tempList = tempList->nextIDList();
            }
        }
};

class Output {
    private:
        Expr* expr;
    public:
        Output(){
            expr = NULL;
        }
        void parse(){
            Parser::S->match(OUTPUT);
            expr = new Expr();
            expr->parse();
            Parser::S->match(SEMICOLON);
        }
        void print(int indent){
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            std::cout << "Output ";
            expr->print();
            std::cout << ";" << std::endl;
        }
        void exec(){
            std::cout << expr->exec() << std::endl;
        }
};

class FuncInvoke {
    private:
        Id* id;
        IdList* idList;
    public:
        FuncInvoke(){
            id = NULL;
            idList = NULL;
        }
        void parse(){
            Parser::S->match(BEGIN);
            if (Parser::S->currentToken() != ID) {
                std::cout << "ERROR: There is an invalid FuncName being parsed " << std::endl;
                exit(0);
            }
            id = new Id(Parser::S->getID());
            Parser::S->nextToken();
            idList = Memory::parseInvokeFunc(id->getIdName());
        }
        void print(int indent){
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            std::cout << "begin ";
            id->print();
            std::cout << "(";
            idList->print();
            std::cout << ");"<< std::endl;
        }
        void exec(){
            Memory::execFunc(id->getIdName());
        }
};

void Stmt::parse(){
            if (Parser::S->currentToken() == ID){
                altNo = 1; s1 = new Assign(); s1->parse(); return;
            }
            if (Parser::S->currentToken() == IF){
                altNo = 2; s2 = new IfThenElse(); s2->parse(); return;
            }
            if (Parser::S->currentToken() == WHILE){
                altNo = 3; s3 = new Loop(); s3->parse(); return;
            }
            if (Parser::S->currentToken() == INPUT){
                altNo = 4; s4 = new Input(); s4->parse(); return;
            }
            if (Parser::S->currentToken() == OUTPUT){
                altNo = 5; s5 = new Output(); s5->parse(); return;
            }
            if (Parser::S->currentToken() == BEGIN){
                altNo = 6; s6 = new FuncInvoke(); s6->parse(); return;
            }
}

void Stmt::print(int indent){
            if (altNo == 1){s1->print(indent);return;}
            if (altNo == 2){s2->print(indent);return;}
            if (altNo == 3){s3->print(indent);return;}
            if (altNo == 4){s4->print(indent);return;}
            if (altNo == 5){s5->print(indent);return;}
            if (altNo == 6){s6->print(indent);return;}
        }
void Stmt::exec(){
            if (altNo == 1){s1->exec();return;}
            if (altNo == 2){s2->exec();return;}
            if (altNo == 3){s3->exec();return;}
            if (altNo == 4){s4->exec();return;}
            if (altNo == 5){s5->exec();return;}
            if (altNo == 6){s6->exec();return;}
        }

void Func::exec(){
    for (int i = 0; i < formalParm.size(); ++i){
        formalParm[i]->assign(arguments->getID()->getValue());
    }
    if (funcBody != NULL)
        funcBody->exec();
    else{
        std::cout << "No Func body can be executed" << std::endl;
        exit(-1);
    }
    std::string firstArg = arguments->getIdNames();
    Memory::endFunc();
    Memory::assignID(firstArg, formalParm[0]->getValue());
}

void Func::getFuncBody(StmtSeq* st){
    funcBody = st;
}

class DeclID {
    private:
        IdList* idList;
    public:
        DeclID(){
            idList = NULL;
        }
        void parse(){
            Parser::S->match(INT);
            idList = new IdList();
            idList->parse();
            idList->declareIDs();
            Parser::S->match(SEMICOLON);
        }
        void print(int indent){
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            std::cout << "int "; 
            idList->print();
            std::cout << std::endl;
        }
        void exec(){
            idList->exec();
        }
};

class DeclFunc {
    private:
        Id* id;
        IdList* idList;
        StmtSeq* stmt_seq;
        Func* func;
    public:
        DeclFunc(){
            id = NULL;
            idList = NULL;
            stmt_seq = NULL;
            func = NULL;
        }
        void parse(){
            if (Parser::S->currentToken() != ID) std::cout << "ERROR: There is an invalid Func ID being declared " << std::endl;
            id = new Id(Parser::S->getID());
            func = new Func(id->getIdName());
            Memory::addFunc(func);
            Parser::S->nextToken();
            if (Parser::S->currentToken() != LPAREN){
                std::cout << "ERROR: This is not a valid Func declaration, you may start the program body without a BEGIN " << std::endl;
                exit(-1);
            }
            Parser::S->match(LPAREN);
            idList = new IdList();
            idList->parse();
            idList->declareIDs();
            Parser::S->match(RPAREN);
            Parser::S->match(BEGIN);
            stmt_seq = new StmtSeq();
            stmt_seq->parse();
            func->getFuncBody(stmt_seq);
            Parser::S->match(ENDFUNC);
            Memory::endFunc();
            Parser::S->match(SEMICOLON);
        }
        void print(int indent){
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            id->print();
            std::cout << "(";
            idList->print();
            std::cout << ")";
            std::cout << " begin" << std::endl;
            stmt_seq->print(indent+2);
            for (int i = 0; i < indent; ++i)
                std::cout << " ";
            std::cout << "endfunc;" << std::endl;
        }
        void exec(){
            //func->exec();
        }
};

class Decl {
    private:
        int altNo;
        DeclID* d1;
        DeclFunc* d2;
    public:
        Decl(){
            altNo = 0;
            d1 = NULL;
            d2 = NULL;
        }
        void parse(){
            if (Parser::S->currentToken() == INT){
                altNo = 1; d1 = new DeclID(); d1->parse(); return;
            }
            else if (Parser::S->currentToken() == ID){
                altNo = 2; d2 = new DeclFunc(); d2->parse(); return;
            }else{
                std::cout << "Missing BEGIN " << std::endl;
                exit(-1);
            }
        }
        void print(int indent){
            if (altNo == 1) {d1->print(indent); return;}
            if (altNo == 2) {d2->print(indent); return;}
        }
        void exec(){
            if (altNo == 1) {d1->exec(); return;}
            if (altNo == 2) {d2->exec(); return;}
        }
};

class DeclSeq {
    private: 
        Decl* decl; 
        DeclSeq* decl_seq;
        //Parser* parser;
    public:
        DeclSeq(){
            decl = NULL;
            decl_seq = NULL;
        }
        void parse(){
            decl = new Decl();
            decl->parse();
            if (Parser::S->currentToken() == BEGIN) return;
            decl_seq = new DeclSeq();
            decl_seq->parse();
        }
        void print(int indent){
            decl->print(indent);
            if (decl_seq != NULL) decl_seq->print(indent);
        }
        void exec(){
            decl->exec();
            if (decl_seq != NULL) decl_seq->exec();
        }
};

class Prog {
    private:
        DeclSeq* decl_seq;
        StmtSeq* stmt_seq;
        int indent;
    public:
        Prog(){
            decl_seq = NULL;
            stmt_seq = NULL;
            indent = 0;
            //parser = P;
        }

        void parse(){
            Parser::S->match(PROGRAM);
            decl_seq = new DeclSeq();
            decl_seq->parse();
            Parser::S->match(BEGIN);
            stmt_seq = new StmtSeq();
            stmt_seq->parse();
            Parser::S->match(END);
            if (Parser::S->currentToken() != 28){
                std::cout << "ERROR: There is something after END " << std::endl;
                exit(-1);
            }
            Parser::S->match(28); //avoid #define EOF=-1
        }

        void print(){
            std::cout << "program " << std::endl; 
            decl_seq->print(indent+2);
            std::cout << "begin " << std::endl; 
            stmt_seq->print(indent+2);
            std::cout << "end" << std::endl;
            
        }

        void exec(){
            decl_seq->exec();
            stmt_seq->exec();
        }
};
 
