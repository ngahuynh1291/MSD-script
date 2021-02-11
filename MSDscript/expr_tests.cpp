//
//  expr_tests.cpp
//  MSDscript
//
//  Created by Nga Huynh on 1/30/21.
//

#include "catch.h"
#include "expr.h"

TEST_CASE("equals") {
    Num* val1 = new Num(17);
    Num* val2 = new Num(24);
    Num* val3 = new Num(17);
    Num* val4 = new Num(24);
    Variable* var1 = new Variable("x");
    Variable* var2 = new Variable("y");
    
    CHECK(val1->equals(new Num(17)) == true);
    CHECK(val3->equals(new Num(24)) == false);
    
    CHECK(var1->equals(new Variable("x")) == true);
    CHECK(var1->equals(new Variable("X")) == false);
    CHECK(var1->equals(var2) == false);

    CHECK(val1->equals(var1) == false);
    CHECK(var2->equals(val2) == false);
    
    CHECK((new Add(val1, val2))->equals(new Add(val3, val4)) == true);
    CHECK((new Add(val1, val2))->equals(new Add(val2, val1)) == false);
    CHECK((new Add(val3, val4))->equals(new Add(val4, val3)) == false);
    
    CHECK((new Mult(val1, val2))->equals(new Mult(val3, val4)) == true);
    CHECK((new Mult(val1, val2))->equals(new Mult(val2, val1)) == false);
    CHECK((new Mult(val3, val4))->equals(new Mult(val4, val3)) == false);
    
    CHECK((new Add(val1, val2))->equals(new Mult(val1, val2)) == false);
    CHECK((new Mult(val1, val2))->equals(new Add(val3, val4)) == false);
    
    CHECK ((new _let("x", val1, new Add(var1, val2)))->equals(new Mult(var1, val1)) == false);
    CHECK ((new _let("x", val1, new Add(var1, val2)))->equals(new _let("x", new Num(17), new Add(new Variable("x"), new Num(24)))) == true);
    CHECK ((new _let("x", val1, new Add(var1, val2)))->equals(new _let("y", new Num(17), new Add(new Variable("x"), new Num(24)))) == false);
    CHECK ((new _let("x", val1, new Add(var1, val2)))->equals(new _let("x", new Num(24), new Add(new Variable("x"), new Num(24)))) == false);
    CHECK ((new _let("x", val1, new Add(var1, val2)))->equals(new _let("x", new Num(17), new Add(new Variable("y"), new Num(24)))) == false);
}

TEST_CASE("interp") {
    Num* val1 = new Num(17);
    Num* val2 = new Num(24);
    Num* val3 = new Num(17);
    Variable* var1 = new Variable("x");
    Variable* var2 = new Variable("y");
    
    CHECK(val1->interp() == 17);
    CHECK(val2->interp() == 24);
    
    CHECK_THROWS_WITH(var1->interp(), "Variable(s) exist(s) in this expression");
    CHECK_THROWS_WITH(var2->interp(), "Variable(s) exist(s) in this expression");
    
    CHECK((new Add(val1, val2))->interp() == 41);
    CHECK((new Add(val1, val3))->interp() == 34);
    CHECK_THROWS_WITH((new Add(val1, var1))->interp(),
                      "Variable(s) exist(s) in this expression");
    CHECK_THROWS_WITH((new Add(var2, val2))->interp(),
                      "Variable(s) exist(s) in this expression");
    
    CHECK((new Mult(val1, val2))->interp() == 408);
    CHECK((new Mult(val1, val3))->interp() == 289);
    CHECK_THROWS_WITH((new Mult(val1, var1))->interp(),
                      "Variable(s) exist(s) in this expression");
    CHECK_THROWS_WITH((new Mult(var2, val2))->interp(),
                      "Variable(s) exist(s) in this expression");
    
    CHECK((new _let("x", val1, val2))->interp() == 24);
    CHECK((new _let("x", val1, new Add(var1, val2)))->interp() == 41);
    CHECK((new _let("x", val1, new _let("y", val2, new Add(var1, val1))))->interp() == 34);
    CHECK((new _let("y", new Add(val1, val2), new Add(var2, new Num(1))))->interp() == 42);
    CHECK((new _let("x", val1, new _let("x", new Add(var1, val2), new Add(var1, new Num(2)))))->interp() == 43);
    CHECK((new _let("x", val1, new _let("x", val2, new Add(var1, new Num(2)))))
          ->interp() == 26);
    CHECK((new _let("x", new Num(17), new Add(new _let("y", new Num(24), new Add(var2, new Num(2))), var1)))->interp() == 43);
   
    CHECK((new Add(new Mult(val1, new _let("x", new Num(1), var1)), val2))->interp() == 41);
    CHECK_THROWS_WITH((new _let("x", new Add(var1, val1), new Add(var1, val2)))->interp(),
                      "Variable(s) exist(s) in this expression");
    
    Expr *let1 = (new _let("x",
                          new Num(1),
                          new Add(new Variable("x"), new Num(2))));
    CHECK( let1->subst("x", new Add(new Variable("y"), new Num(3)))
          ->equals(let1) );
    
    Expr *let2 = (new _let("x",
                          new Variable("x"),
                          new Add(new Variable("x"), new Num(2))));
    CHECK( let2->subst("x", new Add(new Variable("y"), new Num(3)))
          ->equals(new _let("x",
                           new Add(new Variable("y"), new Num(3)),
                           new Add(new Variable("x"), new Num(2)))) );
}

TEST_CASE("has_variables") {
    Num* val1 = new Num(17);
    Num* val2 = new Num(24);
    Num* val3 = new Num(17);
    Num* val4 = new Num(24);
    Variable* var1 = new Variable("x");
    Variable* var2 = new Variable("y");
    Variable* var3 = new Variable("X");
    
    CHECK(val3->has_variable() == false);
    CHECK(val4->has_variable() == false);
    
    CHECK(var1->has_variable() == true);
    CHECK(var3->has_variable() == true);
    
    CHECK((new Add(var1, val1))->has_variable() == true);
    CHECK((new Add(val2, var2))->has_variable() == true);
    CHECK((new Add(var1, var2))->has_variable() == true);
    CHECK((new Add(val1, val2))->has_variable() == false);
    CHECK((new Add(var1, new Add(val2, var2)))->has_variable() == true);
    CHECK((new Add(val1, new Add(val2, val3)))->has_variable() == false);
    
    
    CHECK((new Mult(var1, val1))->has_variable() == true);
    CHECK((new Mult(val2, var2))->has_variable() == true);
    CHECK((new Mult(var1, var2))->has_variable() == true);
    CHECK((new Mult(val1, val2))->has_variable() == false);
    CHECK((new Mult(new Mult(val2, var2), var1))->has_variable() == true);
    CHECK((new Mult(new Mult(val2, val3), val1))->has_variable() == false);
    
    CHECK((new _let("x", new Add(val1, val2), new Add(var1, val3)))
          ->has_variable() == true);
    CHECK((new _let("x", val1, new Add(val2, val3)))->has_variable() == false);
}

TEST_CASE("subst") {
    Num* val1 = new Num(17);
    Num* val2 = new Num(24);
    Variable* var1 = new Variable("x");
    Variable* var2 = new Variable("y");
    Variable* var3 = new Variable("X");
    
    CHECK(val1->subst("x", new Num(24))->equals(val1));
    CHECK(val2->subst("y", new Num(17))->equals(val2));
    
    CHECK(var1->subst("x", new Variable("y"))->equals(var2));
    CHECK(var2->subst("x", new Variable("X"))->equals(var2));
    CHECK(var1->subst("x", new Num(17))->equals(val1));
    CHECK(var2->subst("y", new Add(new Num(17), new Num(24)))->equals(new Add(val1, val2)));
    
    CHECK((new Add(val1, val2))->subst("x", new Variable("x"))->equals(new Add(val1, val2)));
    CHECK((new Add(new Variable("x"), val1))->subst("x", new Variable("y"))
          ->equals(new Add(new Variable("y"), val1)));
    CHECK((new Add(val2, new Variable("x")))->subst("x", new Variable("y"))
          ->equals(new Add(val2, new Variable("y"))));
    CHECK((new Add(new Variable("x"), new Variable("x")))->subst("x", new Variable("y"))
          ->equals(new Add(new Variable("y"), new Variable("y"))));
    CHECK((new Add(new Variable("y"), new Variable("y")))->subst("Y", new Variable("X"))
          ->equals(new Add(new Variable("y"), new Variable("y"))));
    CHECK((new Add(var1, (new Add(val2, var2))))->subst("x", new Variable("X"))
          ->equals(new Add(new Variable("X"), new Add(val2, var2))));
    CHECK((new Add(var1, (new Add(val2, var1))))->subst("x", new Variable("X"))
          ->equals(new Add(new Variable("X"), new Add(val2, new Variable("X")))));
    
    CHECK((new Mult(val1, val2))->subst("x", new Variable("x"))->equals(new Mult(val1, val2)));
    CHECK((new Mult(var1, val1))->subst("x", new Variable("y"))
          ->equals(new Mult(var2, val1)));
    CHECK((new Mult(val2, var1))->subst("x", new Variable("y"))
          ->equals(new Mult(val2, var2)));
    CHECK((new Mult(var1, var1))->subst("x", new Variable("y"))
          ->equals(new Mult(var2, var2)));
    CHECK((new Mult(var2, var2))->subst("Y", new Variable("X"))
          ->equals(new Mult(var2, var2)));
    CHECK((new Mult(new Mult(val2, var2), var1))->subst("x", new Variable("X"))
          ->equals(new Mult(new Mult(val2, var2), var3)));
    CHECK((new Mult(new Mult(val2, var1), var1))->subst("x", new Variable("X"))
          ->equals(new Mult(new Mult(val2, var3), var3)));
    
    CHECK((new _let("x", val1, var1))->subst("x", new Variable("y"))
          ->equals(new _let("x", val1, var1)));
    CHECK((new _let("x", val1, var1))->subst("y", new Variable("X"))
          ->equals(new _let("x", val1, var1)));
    CHECK((new _let("x", new Add(var1, val1), new Add(var1, val2)))->subst("x", new Variable("y"))->equals(new _let("x", new Add(var2, val1), new Add(var1, val2))));
}

TEST_CASE("print") {
    Num* val1 = new Num(17);
    Num* val2 = new Num(24);
    Variable* var1 = new Variable("x");
    Variable* var2 = new Variable("y");
     
    std::string toString;
    
    //Num tests
    toString = "17";
    CHECK(val1->to_str() == toString);
    
    toString = "24";
    CHECK(val2->to_str() == toString);
    
    //Variable tests
    toString = "x";
    CHECK(var1->to_str() == toString);
    
    toString = "y";
    CHECK(var2->to_str() == toString);
    
    //Add tests
    toString = "(17+24)";
    CHECK((new Add(val1, val2))->to_str() == toString);
    
    toString = "(x+y)";
    CHECK((new Add(var1, var2))->to_str() == toString);
 
    toString = "(17+x)";
    CHECK((new Add(val1, var1))->to_str() == toString);
    
    toString = "((17+x)+24)";
    CHECK((new Add(new Add(val1, var1), val2))->to_str() == toString);
    
    toString = "((17*x)+24)";
    CHECK((new Add(new Mult(val1, var1), val2))->to_str() == toString);
 
    toString = "(x+(y+24))";
    CHECK((new Add(var1, new Add(var2, val2)))->to_str() == toString);
 
    toString = "(x+(y*24))";
    CHECK((new Add(var1, new Mult(var2, val2)))->to_str() == toString);
 
    toString = "((17+x)+(24+y))";
    CHECK((new Add(new Add(val1, var1), new Add(val2, var2)))->to_str() == toString);

    toString = "((17*x)+(24*y))";
    CHECK((new Add(new Mult(val1, var1), new Mult(val2, var2)))->to_str() == toString);
    
    //Mult tests
    toString = "(17*24)";
    CHECK((new Mult(val1, val2))->to_str() == toString);
    
    toString = "(x*y)";
    CHECK((new Mult(var1, var2))->to_str() == toString);
 
    toString = "(24*y)";
    CHECK((new Mult(val2, var2))->to_str() == toString);
    
    toString = "((17*x)*24)";
    CHECK((new Mult(new Mult(val1, var1), val2))->to_str() == toString);
    
    toString = "((17+x)*24)";
    CHECK((new Mult(new Add(val1, var1), val2))->to_str() == toString);
 
    toString = "(x*(y*24))";
    CHECK((new Mult(var1, new Mult(var2, val2)))->to_str() == toString);
    
    toString = "(x*(y+24))";
    CHECK((new Mult(var1, new Add(var2, val2)))->to_str() == toString);
 
    toString = "((17+x)*(24+y))";
    CHECK((new Mult(new Add(val1, var1), new Add(val2, var2)))->to_str() == toString);
    
    toString = "((17*x)*(24*y))";
    CHECK((new Mult(new Mult(val1, var1), new Mult(val2, var2)))->to_str() == toString);
    
    //_let tests
    toString = "(_let x=17 _in (x+24))";
    CHECK((new _let("x", val1, new Add(var1, val2)))->to_str() == toString);
    
    toString = "(_let x=5 _in ((_let y=3 _in (y+2))+x))";
    CHECK((new _let("x", new Num(5), new Add((new _let("y", new Num (3), new Add(var2, new Num(2)))), var1)))->to_str() == toString);
}

TEST_CASE("pretty_print") {
    Num* val1 = new Num(17);
    Num* val2 = new Num(24);
    Variable* var1 = new Variable("x");
    Variable* var2 = new Variable("y");

    std::string toPrettyStr;

    //Num tests
    toPrettyStr = "17";
    CHECK(val1->to_pretty_str() == toPrettyStr);
    
    toPrettyStr = "24";
    CHECK(val2->to_pretty_str() == toPrettyStr);
    
    //Variable tests
    toPrettyStr = "x";
    CHECK(var1->to_pretty_str() == toPrettyStr);
    
    toPrettyStr = "y";
    CHECK(var2->to_pretty_str() == toPrettyStr);

    //Add tests
    toPrettyStr = "17 + 24";
    CHECK((new Add(val1, val2))->to_pretty_str() == toPrettyStr);
    
    toPrettyStr = "x + y";
    CHECK((new Add(var1, var2))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "24 + y";
    CHECK((new Add(val2, var2))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "(17 + x) + 24";
    CHECK((new Add(new Add(val1, var1), val2))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "x + y + 24";
    CHECK((new Add(var1, new Add(var2, val2)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "17 * x + 24";
    CHECK((new Add(new Mult(val1, var1), val2))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "x + y * 24";
    CHECK((new Add(var1, new Mult(var2, val2)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "(17 + x) + 24 + y";
    CHECK((new Add(new Add(val1, var1), new Add(val2, var2)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "17 * x + 24 * y";
    CHECK((new Add(new Mult(val1, var1), new Mult(val2, var2)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "(17 + x) + 24 * y";
    CHECK((new Add(new Add(val1, var1), new Mult(val2, var2)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "17 * x + 24 + y";
    CHECK((new Add(new Mult(val1, var1), new Add(val2, var2)))->to_pretty_str() == toPrettyStr);

    //Mult tests
    toPrettyStr = "17 * 24";
    CHECK((new Mult(val1, val2))->to_pretty_str() == toPrettyStr);
    
    toPrettyStr = "x * y";
    CHECK((new Mult(var1, var2))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "24 * y";
    CHECK((new Mult(val2, var2))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "(17 + x) * 24";
    CHECK((new Mult(new Add(val1, var1), val2))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "x * (y + 24)";
    CHECK((new Mult(var1, new Add(var2, val2)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "(17 * x) * 24";
    CHECK((new Mult(new Mult(val1, var1), val2))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "x * y * 24";
    CHECK((new Mult(var1, new Mult(var2, val2)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "(17 + x) * (24 + y)";
    CHECK((new Mult(new Add(val1, var1), new Add(val2, var2)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "(17 * x) * 24 * y";
    CHECK((new Mult(new Mult(val1, var1), new Mult(val2, var2)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "(17 + x) * 24 * y";
    CHECK((new Mult(new Add(val1, var1), new Mult(val2, var2)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "(17 * x) * (24 + y)";
    CHECK((new Mult(new Mult(val1, var1), new Add(val2, var2)))->to_pretty_str() == toPrettyStr);
    
    //_let tests
    toPrettyStr = "_let x = 5\n_in  (_let y = 3\n      _in  y + 2) + x";
    CHECK((new _let("x", new Num(5), new Add(new _let("y", new Num(3), new Add(var2, new Num(2))), var1)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "_let x = 1\n_in  _let x = 2\n     _in  _let x = 3\n          _in  x + 4";
    CHECK((new _let("x", new Num(1), new _let("x", new Num(2), new _let("x", new Num(3), new Add(var1, new Num(4))))))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "_let x = 17\n_in  x + 24";
    CHECK((new _let("x", val1, new Add(var1, val2)))->to_pretty_str()== toPrettyStr);

    toPrettyStr = "5 * (_let x = 5\n     _in  x) + 1";
    CHECK((new Add(new Mult(new Num(5), new _let("x", new Num(5), new Variable("x"))), new Num(1)))->to_pretty_str() == toPrettyStr);

    toPrettyStr = "5 * _let x = 5\n    _in  x + 1";
    CHECK((new Mult(new Num(5), new _let("x", new Num(5), new Add(var1, new Num(1)))))->to_pretty_str() == toPrettyStr);
    
    toPrettyStr = "3 + 7 * _let x = 3\n        _in  _let x = 5\n             _in  x + _let x = 5\n                      _in  x * 1";
    CHECK((new Add(new Num(3),(new Mult(new Num(7),(new _let("x", new Num(3), (new _let("x", new Num(5), new Add(new Variable("x"), new _let("x", new Num(5), new Mult(new Variable("x"), new Num(1))))))))))))->to_pretty_str() == toPrettyStr);
    
    toPrettyStr = "3 + 7 + _let x = 3\n        _in  _let x = 5\n             _in  x + _let x = 5\n                      _in  x * 1";
    CHECK((new Add(new Num(3),(new Add(new Num(7),(new _let("x", new Num(3), (new _let("x", new Num(5), new Add(new Variable("x"), new _let("x", new Num(5), new Mult(new Variable("x"), new Num(1))))))))))))->to_pretty_str() == toPrettyStr);
}
 

