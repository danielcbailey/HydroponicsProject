package common

import "math"

type plantNeedEntry struct {
	low   float64
	high  float64
	count int
}

func computeTargetRange(requirements []plantNeedEntry) (float64, float64) {
	// suppose that numbers between low and high are the 90% region of a normal curve.
	// we convert that to a normal pdf, and we multiply all the pdfs together to get
	// the joint pdf, to which we return the 90% region of.
	meanNumerator := 0.0
	meanDenominator := 0.0
	varianceDenominator := 0.0

	for _, requirement := range requirements {
		reqMean := (requirement.low + requirement.high) / 2.0
		reqVariance := math.Pow((requirement.low-reqMean)/-1.645, 2) // 1.645 is the z-score for 5% of the area under the curve

		meanNumerator += float64(requirement.count) * reqMean / (2 * reqVariance)
		meanDenominator += float64(requirement.count) / (2 * reqVariance)
		varianceDenominator += float64(requirement.count) / reqVariance
	}

	mean := meanNumerator / meanDenominator
	variance := 1 / varianceDenominator

	return mean - 1.645*math.Sqrt(variance), mean + 1.645*math.Sqrt(variance)
}

func GetTargetECRange() (float64, float64) {
	entries := []plantNeedEntry{}

	for _, plant := range GetConfig().SystemConfig.PlantConfigs {
		plantCount := 0
		for _, holder := range GetConfig().SystemConfig.PlantHolders {
			if holder.PlantID == plant.PlantID {
				plantCount++
			}
		}

		entries = append(entries, plantNeedEntry{
			low:   plant.ECLowerLimit,
			high:  plant.ECUpperLimit,
			count: plantCount,
		})
	}

	return computeTargetRange(entries)
}

func GetTargetPHRange() (float64, float64) {
	entries := []plantNeedEntry{}

	for _, plant := range GetConfig().SystemConfig.PlantConfigs {
		plantCount := 0
		for _, holder := range GetConfig().SystemConfig.PlantHolders {
			if holder.PlantID == plant.PlantID {
				plantCount++
			}
		}

		entries = append(entries, plantNeedEntry{
			low:   plant.PHLowerLimit,
			high:  plant.PHUpperLimit,
			count: plantCount,
		})
	}

	return computeTargetRange(entries)
}

func GetTargetLightHoursRange() (float64, float64) {
	entries := []plantNeedEntry{}

	for _, plant := range GetConfig().SystemConfig.PlantConfigs {
		plantCount := 0
		for _, holder := range GetConfig().SystemConfig.PlantHolders {
			if holder.PlantID == plant.PlantID {
				plantCount++
			}
		}

		entries = append(entries, plantNeedEntry{
			low:   plant.LightHoursLowerLimit,
			high:  plant.LightHoursUpperLimit,
			count: plantCount,
		})
	}

	return computeTargetRange(entries)
}
