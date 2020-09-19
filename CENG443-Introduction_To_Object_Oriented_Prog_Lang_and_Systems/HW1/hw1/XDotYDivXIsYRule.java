package hw1;

public class XDotYDivXIsYRule implements Rule {
    private MathExpression premise;
    private MathExpression entails;
    private Var x;
    private Var y;

    XDotYDivXIsYRule(Var x, Var y){
        this.x = x;
        this.y = y;
        Op op = new Op("*", x, y);
        premise = new Op("/", op, x);
        entails = y;
    }

    Var getX(){
        return x;
    }

    Var getY(){
        return y;
    }

    public MathExpression getPremise() {
        return premise;
    }

    public MathExpression getEntails() {
        return entails;
    }
}
