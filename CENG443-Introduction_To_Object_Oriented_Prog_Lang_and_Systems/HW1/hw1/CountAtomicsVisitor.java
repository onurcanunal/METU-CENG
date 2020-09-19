package hw1;

public class CountAtomicsVisitor implements MathVisitor<Integer> {

    public Integer visit(Op op) {
        return op.getFirst().accept(new CountAtomicsVisitor()) + op.getSecond().accept(new CountAtomicsVisitor());
    }

    public Integer visit(Num num) {
        return 1;
    }

    public Integer visit(Sym sym) {
        return 1;
    }

    public Integer visit(Var var) {
        return 1;
    }
}
