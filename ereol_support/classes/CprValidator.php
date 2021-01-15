<?php

// Lifted from https://github.com/itk-dev/cpr-validator

/**
 * Class CprValidator.
 *
 * Validates and scans text for valid Danish CPR numbers.
 */
class CprValidator
{
    /**
     * Check if a string contains a CPR number.
     */
    public function containsCpr(string $text): bool
    {
        return !empty($this->extractCpr($text));
    }

    /**
     * Extract all CPR numbers from a string.
     */
    public function extractCpr(string $text): array
    {
        $cprs = [];

        // We assume that CPR numbers use only space and dash for formatting.
        $pattern = '/(?<=^|\D)\d(?:[ -]?\d){9}(?=\D|$)/';

        if (preg_match_all($pattern, $text, $matches)) {
            foreach ($matches as $match) {
                foreach ($match as $cpr) {
                    if ($this->isCpr($cpr)) {
                        $cprs[] = $cpr;
                    }
                }
            }
        }

        return $cprs;
    }

    /**
     * Check if a string is a valid CPR number.
     */
    public function isCpr(string $cpr): bool
    {
        // Remove space and dash.
        $cpr = preg_replace('/[ -]/', '', $cpr);
        // Check that the cpr consists of 10 decimal digits
        // and contains a valid date
        if (!preg_match('/^\d{10}$/', $cpr)
            || !$this->hasValidDate($cpr)) {
            return false;
        }

        // Check for numbers that does not have a valid modulo 11 control digit.
        if (\in_array(substr($cpr, 0, 6), $this->noModuloCheckNumbers)) {
            return true;
        }

        // Compute weighted sum.
        $digits = array_map('intval', str_split($cpr));
        $weights = [4, 3, 2, 7, 6, 5, 4, 3, 2, 1];
        $weightedSum = 0;
        foreach ($digits as $index => $digit) {
            $weightedSum += $digit * $weights[$index];
        }

        // Check weighted sum.
        return 0 === $weightedSum % 11;
    }

    // Due to lacking capacity of valid modulo 11 numbers some years, there are
    // valid CPR numbers that don't pass modulo 11 check. In those cases we just
    // return true.
    // @See https://cpr.dk/cpr-systemet/personnumre-uden-kontrolciffer-modulus-11-kontrol/
    private $noModuloCheckNumbers = [
        '010160',
        '010164',
        '010165',
        '010166',
        '010169',
        '010170',
        '010174',
        '010180',
        '010182',
        '010184',
        '010185',
        '010186',
        '010187',
        '010188',
        '010189',
        '010190',
        '010191',
        '010192',
    ];

    /**
     * Check if string contains valid CPR number.
     *
     * @deprecated use CprValidator::containsCpr()
     *
     * @param string $cpr
     *   The string to check
     *
     * @return bool
     *   True if string contains a number that could be a valid CPR number
     */
    public function checkCpr(string $cpr): bool
    {
        return $this->containsCpr($cpr);
    }

    /**
     * Check that a candidate CPR number contains a valid date.
     *
     * @param string $cpr
     *   A string that could be a CPR number
     *
     * @return bool return
     *   True if it's considered a date
     */
    private function hasValidDate(string $cpr): bool
    {
        $day = substr($cpr, 0, 2);
        $month = substr($cpr, 2, 2);
        $year = substr($cpr, 4, 2);

        $prefix = (int) $year < 21 ? '20' : '19';
        $year = $prefix.$year;

        return checkdate(intval($month), intval($day), intval($year));
    }
}
