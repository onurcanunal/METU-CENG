import java.util.*;
import java.util.stream.Collectors;
import java.nio.file.Paths;
import java.nio.file.Files;
import java.util.stream.Stream;
import java.io.IOException;
import java.text.DecimalFormat;

public class Covid
{
    // You can add your own variables between them.

    // You can add your own variables between them.

    // You must not change between them.
    private List<List<String>> rows;

    public Covid()
    {
        try
        {
            this.rows = Files
                    .lines(Paths.get("covid19.csv"))
                    .map(row -> Arrays.asList(row.split(",")))
                    .collect(Collectors.toList());
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
    // You must not change between them.

    public void printOnlyCases(String location, String date)
    {
        rows.stream()
                .filter(c -> c.get(1).equals(location))
                .filter(c -> c.get(2).equals(date))
                .forEach(r -> System.out.printf("Result: %d%n", Integer.parseInt(r.get(3)) - Integer.parseInt(r.get(5))));
    }

    public long getDateCount(String location)
    {
        long toReturn = 0;
        toReturn = rows.stream()
                .filter(c -> c.get(1).equals(location))
                .count();

        return toReturn;
    }

    public int getCaseSum(String date)
    {
        int toReturn = 0;

        toReturn = rows.stream()
                .filter(c -> c.get(2).equals(date))
                .mapToInt(r -> Integer.parseInt(r.get(4)))
                .sum();

        return toReturn;
    }

    public long getZeroRowsCount(String location)
    {
        long toReturn = 0;

        toReturn = rows.stream()
                .filter(c -> c.get(1).equals(location))
                .filter(c -> Integer.parseInt(c.get(3)) == 0)
                .filter(c -> Integer.parseInt(c.get(4)) == 0)
                .filter(c -> Integer.parseInt(c.get(5)) == 0)
                .filter(c -> Integer.parseInt(c.get(6)) == 0)
                .count();

        return toReturn;
    }

    public double getAverageDeath(String location)
    {
        double toReturn = 0;

        toReturn = rows.stream()
                .filter(c -> c.get(1).equals(location))
                .mapToDouble(r -> Double.parseDouble(r.get(6)))
                .average()
                .orElse(0.00);

        toReturn = Double.parseDouble(new DecimalFormat("##.00").format(toReturn).replace(",","."));
        return toReturn;
    }

    public String getFirstDeathDayInFirstTenRows(String location)
    {
        String toReturn = null;

        toReturn = rows.stream()
                .filter(c -> c.get(1).equals(location))
                .limit(10)
                .filter(r -> Integer.parseInt(r.get(6)) != 0)
                .filter(r -> Integer.parseInt(r.get(5)) == Integer.parseInt(r.get(6)))
                .findFirst()
                .orElse(Arrays.asList("0", "0", "Not Found"))
                .get(2);

        return toReturn;
    }

    public String[] getDateCountOfAllLocations()
    {
        String[] toReturn = null;

        toReturn = rows.stream()
                .map(c -> Arrays.asList(c.get(0), c.get(1)))
                .distinct()
                .map(r -> r.get(0) + ": " + getDateCount(r.get(1)))
                .toArray(String[]::new);

        return toReturn;
    }

    public List<String> getLocationsFirstDeathDay()
    {
        List<String> toReturn = null;

        toReturn = rows.stream()
                .filter(c -> Integer.parseInt(c.get(5)) != 0)
                .filter(c -> Integer.parseInt(c.get(5)) == Integer.parseInt(c.get(6)))
                .map(r -> r.get(1) + ": " + r.get(2))
                .collect(Collectors.toList());

        return toReturn;
    }

    public String trimAndGetMax(String location, int trimCount)
    {
        String toReturn = null;

        long len = rows.stream()
                .filter(c -> c.get(1).equals(location))
                .count();

        toReturn = rows.stream()
                .filter(c -> c.get(1).equals(location))
                .sorted(Comparator.comparingInt(r -> Integer.parseInt(r.get(4))))
                .limit(len - trimCount)
                .skip(trimCount)
                .max(Comparator.comparingInt(r -> Integer.parseInt(r.get(6))))
                .map(r -> r.get(2) + ": " + r.get(6))
                .orElse("");

        return toReturn;
    }

    public List<List<String>> getOnlyCaseUpDays(String location)
    {
        List<List<String>> toReturn = null;

        toReturn = rows.stream()
                .filter(c -> c.get(1).equals(location))
                .filter(r -> Integer.parseInt(r.get(4)) != 0)
                .collect(Collectors.toList());

        System.out.printf("Result: %d%n", toReturn.size());

        return toReturn;
    }

    public static void main(String[] args)
    {
        // You can test your function in here.
        // This part is not graded.
    }
}
