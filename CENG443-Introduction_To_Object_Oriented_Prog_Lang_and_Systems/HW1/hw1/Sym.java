package hw1;

public class Sym implements MathExpression {
    private String value;

    public Sym(String value){
        this.value = value;
    }

    public String getValue() {
        return value;
    }

    public <T> T accept(MathVisitor<T> visitor) {
        return visitor.visit(this);
    }

    public boolean match(MathExpression me) {
        if((me instanceof Sym) && (this.getValue().equals(((Sym) me).getValue()))){
            return true;
        }
        else{
            return false;
        }
    }
}
