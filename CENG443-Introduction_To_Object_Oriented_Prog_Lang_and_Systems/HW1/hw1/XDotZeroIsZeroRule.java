package hw1;

public class XDotZeroIsZeroRule implements Rule {
    private MathExpression premise;
    private MathExpression entails;
    private Var x;

    XDotZeroIsZeroRule(Var x){
        premise = new Op("*", x, new Num(0));
        entails = new Num(0);
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
