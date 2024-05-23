import csv
import sys
import pandas as pd
from data.dataset.poj104_dataset import POJ104TransformedDataset

error = 0
success = 0

dataset = POJ104TransformedDataset(pd.read_csv("./poj104_llvm.csv"))

with open('poj104_transformed.csv', 'w') as f:
    w = csv.writer(f)
    w.writerow(['id', 'label', 'code'])

    for i in range(len(dataset)):
        data = dataset[i]

        if data['code'] == "Error":
            print('@', end='')
            error += 1
        else:
            print('.', end='')
            success += 1

        sys.stdout.flush()  # Ensure the output is flushed immediately
        w.writerow(data.values())

print()
print(f'Errors: {error}')
print(f'Successes: {success}')

