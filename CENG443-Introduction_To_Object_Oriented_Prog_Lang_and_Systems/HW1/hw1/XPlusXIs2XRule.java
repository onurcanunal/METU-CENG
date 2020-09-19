package hw1;

public class XPlusXIs2XRule implements Rule {
    private MathExpression premise;
    private MathExpression entails;
    private Var x;

    XPlusXIs2XRule(Var x){
        premise = new Op("+", x, x);
        entails = new Op("*", new Num(2), x);
        this.x = x;
    }

    Var getX(){
        return x;
    }

    public MathExpression getPremise() {
        return premise;
    }

    public MathExpression getEntails() {
        return entails;
    }
}
