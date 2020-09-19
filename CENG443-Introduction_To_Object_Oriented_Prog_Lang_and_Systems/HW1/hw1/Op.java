package hw1;

public class Op implements MathExpression {
    private String operand;
    private MathExpression first;
    private MathExpression second;

    public Op(String operand, MathExpression first, MathExpression second){
        this.operand = operand;
        this.first = first;
        this.second = second;
    }

    public String getOperand(){
        return operand;
    }

    public MathExpression getFirst(){
        return first;
    }

    public MathExpression getSecond(){
        return second;
    }

    public <T> T accept(MathVisitor<T> visitor) {
        return visitor.visit(this);
    }

    public boolean match(MathExpression me) {
        if((me instanceof Op) && (this.getOperand() == ((Op) me).getOperand())){
            return (getFirst().match(((Op) me).getFirst()) && getSecond().match(((Op) me).getSecond()));
        }
        else{
            return false;
        }
    }
}
