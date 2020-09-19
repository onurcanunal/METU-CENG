package hw1;

public class ClearVarsVisitor implements MathVisitor<Void> {
    public Void visit(Op op) {
        op.getFirst().accept(new ClearVarsVisitor());
        op.getSecond().accept(new ClearVarsVisitor());
        return null;
    }

    public Void visit(Num num) {
        return null;
    }

    public Void visit(Sym sym) {
        return null;
    }

    public Void visit(Var var) {
        var.setPreviousMatch(null);
        return null;
    }
}
