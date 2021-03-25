import pandas as pd
import plotly.express as px

df = pd.read_csv('plot.csv')
fig = px.line(df, x='Time', y=['AcX', 'AcY', 'AcZ'])
fig.show()
