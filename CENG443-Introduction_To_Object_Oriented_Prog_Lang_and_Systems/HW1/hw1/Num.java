package hw1;

public class Num implements MathExpression {
    private int value;

    public Num(int value){
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public <T> T accept(MathVisitor<T> visitor) {
        return visitor.visit(this);
    }

    public boolean match(MathExpression me) {
        if((me instanceof Num) && (this.getValue() == ((Num) me).getValue())){
            return true;
        }
        else{
            return false;
        }
    }
}
