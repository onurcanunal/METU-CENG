package hw1;

public class PrintMathMLVisitor implements MathVisitor<String> {

    public String visit(Op op) {
        if(!op.getOperand().equals("/")){
            String opStr;
            if(op.getOperand().equals("+")){
                opStr = "+";
            }
            else if(op.getOperand().equals("*")){
                opStr = "&times;";
            }
            else{
                opStr = "&vdash;";
            }
            return "<mrow><mo>(</mo>" +
                    op.getFirst().accept(new PrintMathMLVisitor()) +
                    "<mo>" + opStr + "</mo>" +
                    op.getSecond().accept(new PrintMathMLVisitor()) + "<mo>)</mo></mrow>";
        }
        else {
            return "<mrow><mfrac>" +
                    op.getFirst().accept(new PrintMathMLVisitor()) + op.getSecond().accept(new PrintMathMLVisitor()) +
                    "</mfrac></mrow>";
        }
    }

    public String visit(Num num) {
        return "<mrow><mn>" + num.getValue() + "</mn></mrow>";
    }

    public String visit(Sym sym) {
        return "<mrow><mi>" + sym.getValue() + "</mi></mrow>";
    }

    public String visit(Var var) {
        if(var.getPreviousMatch() == null){
            return "<mrow><msub><mi>V</mi><mn>" +
                    var.getId() +
                    "</mn></msub></mrow>";
        }
        else{
            return "<mrow><msub><mi>V</mi><mn>" +
                    var.getId() +
                    "</mn></msub>" +
                    "<mo>[</mo>"+ var.getPreviousMatch().accept(new PrintMathMLVisitor()) +
                    "<mo>]</mo></mrow>";
        }
    }
}
